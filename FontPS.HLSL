Texture2D shaderTexture;
SamplerState SampleType;
 
cbuffer PixelBuffer
{
    float4 pixelColor;
};
 
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};
 

float4 FontPixelShader(PixelInputType input) : SV_TARGET
{
    float4 color;
    
    // 이 위치에서 텍스쳐 픽셀을 샘플링한다
    color = shaderTexture.Sample(SampleType, input.tex);
    
    // 텍스쳐의 색상이 검은 색이면 이 픽셀을 투명으로 처리한다.
    if (color.r == 0.0f)
    {
        color.a = 0.0f;
    }
    else
    {
        // 텍스쳐의 검은 색이 아닌 경우 글꼴의 픽셀이므로 글꼴 픽셀 색상을 사용하여 그린다.
        color.rgb = pixelColor.rgb;
        color.a = 1.0f;
    }
 
    return color;
}