#include "Sampler.HLSLi"
#include "Lighting.HLSLi"
#include "Shading.HLSLi"
#include "BRDF.HLSLi"


struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

cbuffer DeferredCBuffer : register(b3)
{
    matrix inverseProjection;
    matrix inverseView;
    int useAO;
    int useEnvMap;
    int deferredCBufferpadding1;
    int deferredCBufferpadding2;
};

Texture2D DepthTexture : register(t0);
Texture2D Diffuse : register(t1);
Texture2D MetalRough : register(t2);
Texture2D Normals : register(t3);
// t4 : shadowMap
Texture2D AO : register(t5);


TextureCube EnvMap : register(t6);
TextureCube PrefilteredSpecMap : register(t7);
Texture2D BrdfLUT : register(t8);



float3 CalculateWorldFromDepth(float depth, float2 texCoord)
{
    // 0, 1 범위의 texCoord 를 -1, 1 범위로 변경하고, 상하 반전하여 clip space 좌표로 만듦.
    float2 clipXY = texCoord * 2.0 - 1.0;
    clipXY.y = -clipXY.y;
    
    // clip space 좌표에 inverseProjection 를 곱하여 view space 좌표로 변경
    float4 clipSpace = float4(clipXY, depth, 1);
    float4 viewSpace = mul(inverseProjection, clipSpace);
    
    // perspective divide
    // 4D 좌표에서 (1,1,1,1) (2,2,2,2) (3,3,3,3) 은 모두 같은 값이다.
    // (3,3,3,3) 을 w 값으로 나누면 (1,1,1,1) 이 된다.
    viewSpace /= viewSpace.w;
    
    // viewSpace 좌표에 inverseView 를 곱하여 world space 좌표로 변경
    float4 worldSpace = mul(inverseView, viewSpace);
    return worldSpace.xyz;
}



float4 DeferredPixelShader(PixelInputType input) : SV_TARGET
{
    float depth = DepthTexture.Sample(PointSampler, input.tex).r;
    float3 posW = CalculateWorldFromDepth(depth, input.tex);
    float4 matMetalRough = MetalRough.Sample(PointSampler, input.tex);
    float3 albedo = Diffuse.Sample(PointSampler, input.tex).rgb;
    float metallic = matMetalRough.r;
    float roughness = matMetalRough.g;
    float occlusion = matMetalRough.b;
    float3 normal = Normals.Sample(PointSampler, input.tex).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    
    SurfaceInfo surf;
    surf.posW = float4(posW, 1);
    surf.N = normal;
    surf.V = normalize(eyePosition.xyz - posW);
    surf.NdotV = dot(surf.N, surf.V);
    
    float3 Lo = float3(0, 0, 0);
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic);
    
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        Light light = Lights[i];
        if (light.status == LIGHT_DISABLED)
        {
            continue;
        }
        
        LightingInfo li = EvalLightingInfo(surf, light, i);
        
        float NDF = DistributionGGX(saturate(li.NdotH), roughness);
        float G = GeometrySmith(saturate(surf.NdotV), saturate(li.NdotL), roughness);
        float3 F = fresnelSchlick(saturate(dot(li.H, surf.V)), F0);
        float3 kS = F;
        float3 kD = float3(1.0, 1.0, 1.0) - kS;
        kD *= 1.0 - metallic;
        
        float NdotL = saturate(li.NdotL);
        
        float3 numerator = NDF * G * F;
        float denominator = 4.0 * saturate(surf.NdotV) * NdotL;
        float3 specular = numerator / max(denominator, 0.001);
        
        Lo += (kD * albedo / PI + specular) * light.color.rgb * li.attenuation * NdotL * (li.shadowFactor);
    }
    
    float ambient = globalAmbient.rgb * albedo;
    if (/*useEnvMap*/ false)
    {
        float3 kS = fresnelSchlickRoughness(saturate(surf.NdotV), F0, roughness);
        float3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;
        float3 irradiance = EnvMap.Sample(LinearSampler, surf.N).rgb;
        float3 diffuse = irradiance * albedo;

        float3 R = normalize(reflect(-surf.V, surf.N));
        float3 prefilterdColour = PrefilteredSpecMap.SampleLevel(LinearSampler, R, roughness * 5.0).rgb;
        float2 envBrdf = BrdfLUT.Sample(PointSampler, float2(saturate(surf.NdotV), roughness)).rg;
        float3 specular = prefilterdColour * (kS * envBrdf.x + envBrdf.y);
        ambient = (kD * diffuse + specular);
    }
    
    
    float ao = useAO ? AO.Sample(PointSampler, input.tex).r : 1.0;
    ambient *= ao * occlusion;

    float3 colour = ambient + Lo;
    
    return float4(colour, 1.0);
}