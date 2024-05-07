
// 전역변수

// 텍스쳐
Texture2D ShaderTexture;

// C++ 에서 설정할 수 있음
// 어떻게 텍스쳐의 픽셀이 씌워지는 지 수정함
SamplerState SampleType;

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
    float4 TextureColor;
    
    // 이 텍스쳐 좌표 위치에서 샘플러를 사용하여 텍스쳐에서 픽셀 색깔을 샘플링한다.
    TextureColor = ShaderTexture.Sample(SampleType, input.tex);
    
    return TextureColor;
}