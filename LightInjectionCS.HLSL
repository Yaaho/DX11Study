#include "Common.HLSLi"

#define MAX_LIGHTS 4

#define LOCAL_SIZE_X 8
#define LOCAL_SIZE_Y 8
#define LOCAL_SIZE_Z 1
#define M_PI 3.14159265359
#define EPSILON 0.0001f

RWTexture3D<float4> output : register(u0);

SamplerState LinearSampler : register(s0);

cbuffer LightInjectionCBuffer : register(b0)
{
    matrix prevView;
    matrix prevProjection;
    matrix invView;
    matrix invProjection;
    float4 cameraPosition;
    float4 biasNearFarPow;
    float4 anisoDensityScatteringAbsorption;
    int accumulation;
    int groupX;
    int groupY;
    int groupZ;
}

struct Light
{
    float4 position;
    float4 direction;
    float4 color;

    float constantAtt;
    float linearAtt;
    float quadAtt;
    float spotAngle;

    int lightType;
    int status;
    int lightpadding1;
    int lightpadding2;
};

cbuffer LightProperties : register(b1)
{
    float4 eyePosition;
    float4 globalAmbient;
    Light Lights[MAX_LIGHTS];
}

struct ShadowMapMatrix
{
    float shadowMapTopLeftX;
    float shadowMapTopLeftY;
    float shadowMapTextureRatio;
    float shadowMappadding;

    matrix lightViewProjection;
};

cbuffer ShadowMapProperties : register(b2)
{
    float mapWidth;
    float mapHeight;
    float shadowMapProppadding1;
    float shadowMapProppadding2;

    ShadowMapMatrix m_ShadowMaps[MAX_LIGHTS];
};

Texture3D History : register(t0);
Texture2D ShadowMap : register(t1);
Texture2D BlueNoise : register(t2);

float visibility(float3 p, int lightIndex)
{
    // Transform frag position into Light-space.
    float4 lightSpacePosition = mul(float4(p, 1), m_ShadowMaps[lightIndex].lightViewProjection);
    
    // Perspective divide
    float3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    float currentDepth = projCoords.z;

    // 0 - 1 범위로 변경
    projCoords = projCoords * 0.5 + 0.5;
    
    // 상하 반전
    projCoords.y = 1 - projCoords.y;

    // 여기까진 문제 없다.

    // projCoords 가 섀도 맵 아틀러스에서 광원에 해당하는 coord 를 가리키도록 함
    projCoords.x *= m_ShadowMaps[lightIndex].shadowMapTextureRatio;
    projCoords.y *= m_ShadowMaps[lightIndex].shadowMapTextureRatio;
    
    projCoords.x += m_ShadowMaps[lightIndex].shadowMapTopLeftX;
    projCoords.y += m_ShadowMaps[lightIndex].shadowMapTopLeftY;
    
    float minU = m_ShadowMaps[lightIndex].shadowMapTopLeftX;
    float minV = m_ShadowMaps[lightIndex].shadowMapTopLeftY;
    float maxU = minU + m_ShadowMaps[lightIndex].shadowMapTextureRatio;
    float maxV = minV + m_ShadowMaps[lightIndex].shadowMapTextureRatio;
    
    
    if (any(GreaterThan(projCoords.xy, float2(maxU, maxV)) || any(LessThan(projCoords.xy, float2(minU, minV)))))
        return 1.0f;
    
    float bias = biasNearFarPow.x;
    
    // 포인트 샘플러를 사용하여 깊이 텍스처의 값을 샘플링합니다.
    float closestDepth = ShadowMap.SampleLevel(LinearSampler, projCoords.xy, 0).r;
    
    // 가까울수록 0 멀수록 1
    // currentDepth 가 closestDepth 보다 더 멀면 빛이 비추지 않는 곳이므로 0을 할당
    // bias 가 너무 큰 값이라 오류가 생겼었다.
    float visibilityValue = (closestDepth < currentDepth /* - bias*/) ? 0.0f : 1.0f;
    
    return visibilityValue;
}



float phase_function(float3 Wo, float3 Wi, float g)
{
    float cos_theta = dot(Wo, Wi);
    float denom = 1.0f + g * g + 2.0f * g * cos_theta;
    return (1.0f / (4.0f * M_PI)) * (1.0f - g * g) / max(pow(denom, 1.5f), EPSILON);
}

float z_slice_thickness(int z, int maxz)
{
    //return 1.0f; //linear depth
    return exp(-float(maxz - z - 1) / float(maxz));
}


float sample_blue_noise(uint3 coord)
{
    // coordZ 값에 따라 noise 의 y offset 이 1씩 올라간다.
    uint2 noise_coord = (coord.xy + uint2(0, 1) * coord.z) % BLUE_NOISE_TEXTURE_SIZE;
    // (coord.xy + uint2(0, 1) * coord.z * BLUE_NOISE_TEXTURE_SIZE) % BLUE_NOISE_TEXTURE_SIZE;

    return BlueNoise.Load(int3(noise_coord, 0)).r;
}


[numthreads(LOCAL_SIZE_X, LOCAL_SIZE_Y, LOCAL_SIZE_Z)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint3 coord = DTid.xyz;
    
    uint3 max;
    History.GetDimensions(max.x, max.y, max.z);
    
    if (all(LessThan(coord, float3(max))))
    {
        // Get jitter for the current pixel, remapped to -0.5 to +0.5 range.
        float jitter = (sample_blue_noise(coord) - 0.5f) * 0.999f;
        
        // Get the world position of the current voxel.
        // Worldpos 는 문제가 없다.
        float3 world_pos = ConvertThreadIdToWorld(coord, max, invView, invProjection, biasNearFarPow.w, biasNearFarPow.y, biasNearFarPow.z, jitter);
        
        // Get the view direction from the current voxel.
        float3 toCamera = normalize(cameraPosition.xyz - world_pos);

        // Density and coefficient estimation.
        float thickness = z_slice_thickness(coord.z, max.z);
        float density    = anisoDensityScatteringAbsorption.y;
        
        float3 lighting = float3(0.0f, 0.0f, 0.0f);
        float4 color_and_density;
        

        // 일단 디렉셔널 라이트 하나만 지원하자
        for (int i = 0; i < 1; i++)
        {
            Light light = Lights[i];
            
            // Perform lighting.
            lighting = light.color.rgb * light.color.a;

            float visibility_value = visibility(world_pos, i);

            if (visibility_value > EPSILON)
                lighting += visibility_value * light.color.xyz * phase_function(toCamera, -light.direction.xyz, anisoDensityScatteringAbsorption.x);
            
            // RGB = Amount of in-scattered light, A = Density.
            color_and_density = float4(lighting * density, density);
        }
        
        // Temporal accumulation
        if (accumulation)
        {
            float nojittter = 0.0f;
            
            float3 world_pos_withoutjitter = ConvertThreadIdToWorld(coord, max, invView, invProjection, biasNearFarPow.w, biasNearFarPow.y, biasNearFarPow.z, nojittter);

            // Find the history UV
            float3 history_uv = ConvertWorldToUV(world_pos_withoutjitter, prevView, prevProjection, biasNearFarPow.w, biasNearFarPow.y, biasNearFarPow.z);
            
            // If history UV is outside the frustum, skip history
            if (all(history_uv >= float3(0.0f, 0.0f, 0.0f)) && all(history_uv <= float3(1.0f, 1.0f, 1.0f)))
            {
                // Fetch history sample
                float4 history = History.SampleLevel(LinearSampler, history_uv, 0);

                color_and_density = lerp(history, color_and_density, 0.05f);
            }
        }
        
        output[coord] = color_and_density;
    }
}
