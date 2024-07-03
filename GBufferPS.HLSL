#include "Lighting.HLSLi"
#include "Shading.HLSLi"

#define VERTEX_NORMAL 0
#define NORMAL_MAP 1
#define BUMP_MAP 2

Texture2D Albedo : register(t0);
Texture2D NormalMap : register(t1);
Texture2D OcclusionRoughnessMetal : register(t2);
Texture2D AoMap : register(t3);
Texture2D Emissive : register(t4);

cbuffer PBRMaterialBuffer : register(b0)
{
    float4 gAlbedo;
    float gMetallic;
    float gRoughness;
    
    int gUseAlbedoMap;
    int gUseOccMetalRough;
    int gUseAoMap;
    int gUseEmmisive;
    int gNormalState;
    int gConvertToLinear;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 pos : POSITION0;
    float4 wPosition : POSITION1;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct GBufferOutput
{
    float4 diffuse : SV_TARGET0;
    float4 metalRoughOcclusion : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 emissive : SV_TARGET3;
};

GBufferOutput GBufferPixelShader(PixelInputType input)
{
    GBufferOutput output;
    
    SurfaceInfo surf;
    surf.posW = input.wPosition;
    surf.N = normalize(input.normal);
    surf.T = normalize(input.tangent);
    surf.B = normalize(input.binormal);
    surf.V = normalize(eyePosition.xyz - surf.posW.xyz);
    surf.NdotV = dot(surf.N, surf.V);
    
    // 노말 맵을 사용할 때
    if (gNormalState == NORMAL_MAP)
    {
        surf.N = CalcNormalFromNormMap(NormalMap, input.texCoord, surf);
    }
    // 범프 맵을 사용할 때
    else if (gNormalState == BUMP_MAP)
    {
        surf.N = CalcNormalFromBumpMap(NormalMap, input.texCoord, surf);
    }
    
    // 이 아래부터 GBuffer 를 채워넣는다.
    
    // 기본 색상
    float4 albedo = gAlbedo;
    if (gUseAlbedoMap)
    {
        albedo = Albedo.Sample(LinearSampler, input.texCoord);
        if (gConvertToLinear)
        {
            albedo = SRGBtoLINEAR(albedo);
        }
    }
    
    float occlusion = 1;
    
    float metallic = gMetallic;
    float roughness = gRoughness;
    if (gUseOccMetalRough)
    {
        float3 occRoughMetal = OcclusionRoughnessMetal.Sample(LinearSampler, input.texCoord).rgb;
        occlusion = occRoughMetal.r;
        roughness = occRoughMetal.g;
        metallic = occRoughMetal.b;
    }
    
    if (gUseAoMap)
    {
        occlusion = AoMap.Sample(LinearSampler, input.texCoord).r;
    }
    
    float4 emissive = float4(0.0, 0.0, 0.0, 0.0);
    if (gUseEmmisive)
    {
        emissive = Emissive.Sample(LinearSampler, input.texCoord);
        if (gConvertToLinear)
        {
            emissive = SRGBtoLINEAR(emissive);
        }
    }
    
    output.diffuse = pow(float4(albedo.rgb, 0), 2.2);
    output.metalRoughOcclusion.r = metallic;
    output.metalRoughOcclusion.g = roughness;
    output.metalRoughOcclusion.b = occlusion;
    output.normal = float4(surf.N * 0.5 + 0.5, 1);
    output.emissive = emissive;
    
    return output;
}