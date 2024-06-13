Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D depthMapTexture2 : register(t2);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);
 

cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float4 diffuseColor2;
};
 

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
    float4 lightViewPosition2 : TEXCOORD3;
    float3 lightPos2 : TEXCOORD4;
};
 
 
float4 ShadowPixelShader(PixelInputType input) : SV_TARGET
{
    float bias;
    float4 color;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;
 
 
    // 부동 소수점 정밀도 문제를 해결할 바이어스 값을 설정합니다.
    bias = 0.001f;
 
    // 모든 픽셀에 대해 기본 출력 색상을 주변 광원 값으로 설정합니다.
    color = ambientColor;
 
    // 투영 된 텍스처 좌표를 계산합니다.
    projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;
 
    // 투영 된 좌표가 0에서 1 범위에 있는지 결정합니다. 그렇다면 이 픽셀은 빛이 보는 시점의 안에 있다.
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        // 투영 된 텍스처 좌표 위치에서 샘플러를 사용하여 깊이 텍스처에서 섀도우 맵 깊이 값을 샘플링합니다.
        depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;
 
        // 빛이 바라보는 시점에서 이 픽셀의 깊이를 계산한다.
        lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
 
        // lightDepthValue에서 바이어스를 뺍니다.
        lightDepthValue = lightDepthValue - bias;
 
        // 섀도우 맵 값의 깊이와 빛의 깊이를 비교하여이 픽셀을 음영 처리할지 조명할지 결정합니다.
        // 빛의 시점에서 본 이 픽셀의 깊이 lightDepthValue 가 
        // depthMap 에서 이 픽셀이 가리키는 UV 를 통해 받아온 depthValue 보다 작으면 이 픽셀에는 빛이 비추는 것이다.
        // 빛이 비추는 픽셀에서는 diffuse 를 계산하고 비추지 않는 픽셀은 ambientColor 만 갖게 된다.  
        if (lightDepthValue < depthValue)
        {
            // 이 픽셀의 빛의 양을 계산합니다.
            lightIntensity = saturate(dot(input.normal, input.lightPos));
 
            if (lightIntensity > 0.0f)
            {
                // 확산 색과 광 강도의 양에 따라 최종 확산 색을 결정합니다.
                color += (diffuseColor * lightIntensity);
            }
        }
    }
 
    // 두번째 조명
    projectTexCoord.x = input.lightViewPosition2.x / input.lightViewPosition2.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.lightViewPosition2.y / input.lightViewPosition2.w / 2.0f + 0.5f;
 
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        depthValue = depthMapTexture2.Sample(SampleTypeClamp, projectTexCoord).r;
 
        lightDepthValue = input.lightViewPosition2.z / input.lightViewPosition2.w;
        lightDepthValue = lightDepthValue - bias;
 
        if (lightDepthValue < depthValue)
        {
            lightIntensity = saturate(dot(input.normal, input.lightPos2));
            if (lightIntensity > 0.0f)
            {
                color += (diffuseColor2 * lightIntensity);
            }
        }
    }
    
    // 최종 빛의 색상을 채웁니다.
    color = saturate(color);
 
    // 이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다.
    textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);
 
    // 빛과 텍스처 색상을 결합합니다.
    color = color * textureColor;
 
    return color;
}