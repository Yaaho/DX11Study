#ifndef LIGHTING_COMMON
#define LIGHTING_COMMON

#define MAX_LIGHTS 4

struct Light
{
    float4 position;
    float4 direction;
    float4 color;
    
    float constantAtt;
    float linearAtt;
    float quadAtt;
    float spotAngle;
    
    float shadowMapTopLeftX;
    float shadowMapTopLeftY;
    float shadowMapTextureRatio;
    
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
    matrix m_lightViewMatrix;
    matrix m_lightProjectionMatrix;
};

cbuffer ShadowMapProperties : register(b2)
{
    ShadowMapMatrix m_ShadowMaps[MAX_LIGHTS];
};




#endif