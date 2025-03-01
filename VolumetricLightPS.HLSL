#include "Sampler.HLSLi"
#include "Common.HLSLi"

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

cbuffer VolumetricLightCBuffer : register(b0)
{
    matrix View;
    matrix Projection;
    matrix invView;
    matrix invProjection;
    float depthPower;
    float nearPlaneDistance;
    float farPlaneDistance;
    float padding;
};

Texture2D DepthTexture : register(t0);
Texture3D VoxelGrid : register(t1);


float3 CalculateWorldFromDepth(float depth, float2 texCoord)
{
    // 0, 1 범위의 texCoord 를 -1, 1 범위로 변경하고, 상하 반전하여 clip space 좌표로 만듦.
    float2 clipXY = texCoord * 2.0 - 1.0;
    clipXY.y = -clipXY.y;
    
    // clip space 좌표에 inverseProjection 를 곱하여 view space 좌표로 변경
    float4 clipSpace = float4(clipXY, depth, 1);
    float4 viewSpace = mul(invProjection, clipSpace);
    
    // perspective divide
    // 4D 좌표에서 (1,1,1,1) (2,2,2,2) (3,3,3,3) 은 모두 같은 값이다.
    // (3,3,3,3) 을 w 값으로 나누면 (1,1,1,1) 이 된다.
    viewSpace /= viewSpace.w;
    
    // viewSpace 좌표에 inverseView 를 곱하여 world space 좌표로 변경
    float4 worldSpace = mul(invView, viewSpace);
    
    worldSpace.xyz /= worldSpace.w;
    
    return worldSpace.xyz;
}


float4 VolumetricLightPixelShader(PixelInputType input) : SV_TARGET
{
    float depth = DepthTexture.Sample(PointSampler, input.tex).r;
    float3 posW = CalculateWorldFromDepth(depth, input.tex);

    float3 UV = ConvertWorldToUV(posW, View, Projection, depthPower, nearPlaneDistance, farPlaneDistance);

    float4 scatteringColorAndTransmittance = VoxelGrid.Sample(LinearSampler, UV);
    
    return scatteringColorAndTransmittance;
}