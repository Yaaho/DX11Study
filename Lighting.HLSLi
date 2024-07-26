#ifndef LIGHTING_COMMON
#define LIGHTING_COMMON

#include "Sampler.HLSLi"

#define MAX_LIGHTS 4

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

#define LIGHT_DISABLED 0
#define LIGHT_ENABLED 1
#define LIGHT_ENABLED_W_SHADOWMAP 2


// shadow map atlas
Texture2D ShadowMap : register(t4);


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
    float padding;
    
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
    
};

cbuffer ShadowMapProperties : register(b2)
{
    float mapWidth;
    float mapHeight;
    float padding1;
    float padding2;
    
    ShadowMapMatrix m_ShadowMaps[MAX_LIGHTS];
};

float ShadowFactor(float4 worldPosition, int lightIndex)
{
    float4 lightSpacePosition = mul(worldPosition, m_ShadowMaps[lightIndex].lightViewMatrix);
    lightSpacePosition = mul(lightSpacePosition, m_ShadowMaps[lightIndex].lightProjectionMatrix);
    
    float3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    float currentDepth = projCoords.z;
    
    if (currentDepth > 1)
    {
        return 0;
    }
    
    // 0 - 1 범위로 변경
    projCoords = (projCoords + 1) / 2.0;
    // 좌표의 상하 반전
    projCoords.y = -projCoords.y; 
    
    // projCoords 가 이 광원에 해당하는 cord 를 가리키도록 함
    projCoords *= m_ShadowMaps[lightIndex].shadowMapTextureRatio;
    projCoords.x += m_ShadowMaps[lightIndex].shadowMapTopLeftX;
    projCoords.y += m_ShadowMaps[lightIndex].shadowMapTopLeftY;
    
    float2 atlasSize = float2(1, 1) /
    float2(mapWidth * m_ShadowMaps[lightIndex].shadowMapTextureRatio,
    mapHeight * m_ShadowMaps[lightIndex].shadowMapTextureRatio);
    
    float shadow = 0;
    float epsilon = 0.01f;
    
    for (int x = -1; x < 2; ++x)
    {
        for (int y = -1; y < 2; ++y)
        {
            float closeestDepth = ShadowMap.Sample(LinearSampler, projCoords.xy + float2(x, y) * atlasSize).r;
            shadow += (closeestDepth < currentDepth - epsilon);
        }
    }
    
    shadow /= 9;
    return shadow;
}

#endif