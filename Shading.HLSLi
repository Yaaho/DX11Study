#ifndef SHADING_COMMON
#define SHADING_COMMON

#include "Sampler.HLSLi"


struct SurfaceInfo
{
    float4 posW;
    float3 N;
    float3 T;
    float3 B;
    float3 V;
    float NdotV;
};

inline float3 CalcNormalFromNormMap(Texture2D normalMap, float2 uv, SurfaceInfo surf)
{
    float3 normalMapVal = normalMap.Sample(LinearSampler, uv).xyz;
    // 노말 값의 범위를 (0, +1) 에서 (-1, +1)로 확장한다.
    normalMapVal = normalize((normalMapVal * 2) - 1.0);
    
    // 노말 맵의 데이터에서 법선을 계산한다.
    float3 N =
    (normalMapVal.x * surf.T) +
    (normalMapVal.y * surf.B) +
    (normalMapVal.z * surf.N);
    
    return normalize(N);
}

inline float3 CalcNormalFromBumpMap(Texture2D bumpMap, float2 uv, SurfaceInfo surf)
{
    float3 N = surf.N;
    float2 bumpTextureSize;
    
    // bumpMap 텍스쳐의 크기를 읽는다.
    bumpMap.GetDimensions(bumpTextureSize[0], bumpTextureSize[1]);
    
    // 범프맵 픽셀 하나의 UV 크기를 구함
    float2 pixelSize = 1.0 / bumpTextureSize;
    
    // 범프맵의 주변 픽셀들의 값을 이용하여 법선 생성
    float mid = bumpMap.Sample(PointSampler, uv) * 2.0 - 1.0;
    float left = bumpMap.Sample(PointSampler, uv + float2(-pixelSize.x, 0)) * 2.0 - 1.0;
    float right = bumpMap.Sample(PointSampler, uv + float2(pixelSize.x, 0)) * 2.0 - 1.0;
    float top = bumpMap.Sample(PointSampler, uv + float2(0, -pixelSize.y)) * 2.0 - 1.0;
    float bottom = bumpMap.Sample(PointSampler, uv + float2(0, -pixelSize.y)) * 2.0 - 1.0;
    float3 p1 = ((bottom - mid) - (top - mid)) * normalize(surf.B);
    float3 p2 = ((left - mid) - (right - mid)) * normalize(surf.T);
    
    return normalize(N - (p1 + p2));
}


// H 는 normalize(lightDir + viewDir) 값이다.


static const float GAMMA = 2.2;
static const float INV_GAMMA = 1.0 / GAMMA;


// linear to sRGB approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
float3 LINEARtoSRGB(float3 color)
{
    return pow(color, INV_GAMMA);
}


// sRGB to linear approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
float4 SRGBtoLINEAR(float4 srgbIn)
{
    return float4(pow(srgbIn.xyz, GAMMA), srgbIn.w);
}


#endif