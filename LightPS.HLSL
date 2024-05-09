// 전역변수

// 텍스쳐
Texture2D ShaderTexture;

// C++ 에서 설정할 수 있음
// 어떻게 텍스쳐의 픽셀이 씌워지는 지 수정함
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 diffusColor;
    float3 lightDirection;
    float padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 TextureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

    // 이 텍스쳐 좌표 위치에서 샘플러를 사용하여 텍스쳐에서 픽셀 색깔을 샘플링한다.
    TextureColor = ShaderTexture.Sample(SampleType, input.tex);
    
    // 법선 벡터와 내적을 하기 전 light 벡터의 방향을 반대로 바꾼다.
    lightDir = -lightDirection;
    
    // 뒤집은 lightDirection 벡터와 법선 벡터를 내적하여 빛의 양을 계산한다.
    lightIntensity = saturate(dot(input.normal, lightDir));
    
    // 빛의 강도와 빛의 색을 곱해 빛의 색상을 결정한다.
    color = saturate(diffusColor * lightIntensity);
    
    // 빛의 색과 텍스쳐의 색을 곱해서 이 픽셀의 최종 색상을 결정한다.
    color = color * TextureColor;
    
    return color;
}