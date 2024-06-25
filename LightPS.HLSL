Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer
{
    float3 lightDirection;
    float padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 colors;
    float4 normals;
    float3 lightDir;
    float lightIntensity;
    float4 outputColor;
    
    // 이 텍스쳐 좌표 위치의 컬러 렌더링 텍스쳐에서 색상을 샘플링한다.
    colors = colorTexture.Sample(SampleTypePoint, input.tex);
    
    // 이 텍스쳐 좌표 위치의 normal 렌더링 텍스쳐에서 법선을 샘플링한다.
    normals = normalTexture.Sample(SampleTypePoint, input.tex);
    
    // 내적 계산을 위해 빛 방향을 반전시킨다.
    lightDir = -lightDirection;
    
    // 노말과 빛의 방향 벡터를 내적해 이 픽셀의 빛의 양을 계산한다.
    lightIntensity = saturate(dot(normals.xyz, lightDir));
    
    // lightIntensity 와 결합된 픽셀의 색상을 기반으로 최종 diffuse color 를 계산한다.
    outputColor = saturate(colors * lightIntensity);
    
    return outputColor;
}