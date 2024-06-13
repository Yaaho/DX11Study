cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};
 
cbuffer ScreenSizeBuffer
{
    float screenWidth;
    float3 padding;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};
 
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float2 texCoord1 : TEXCOORD1;
    float2 texCoord2 : TEXCOORD2;
    float2 texCoord3 : TEXCOORD3;
    float2 texCoord4 : TEXCOORD4;
    float2 texCoord5 : TEXCOORD5;
    float2 texCoord6 : TEXCOORD6;
    float2 texCoord7 : TEXCOORD7;
    float2 texCoord8 : TEXCOORD8;
    float2 texCoord9 : TEXCOORD9;
};
 
 
PixelInputType HorizontalBlurVertexShader(VertexInputType input)
{
    PixelInputType output;
    float texelSize;

 
    // 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;
 
    // 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 ??계산합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 픽셀 쉐이더의 텍스처 좌표를 저장한다.
    output.tex = input.tex;
    
    // 한 픽셀의 가로 float 크기를 구함
    texelSize = 1.0f / screenWidth;
 
    // blur 작업에 사용할 이웃한 가로 8개 픽셀의 UV 위치를 정함.
    output.texCoord1 = input.tex + float2(texelSize * -4.0f, 0.0f);
    output.texCoord2 = input.tex + float2(texelSize * -3.0f, 0.0f);
    output.texCoord3 = input.tex + float2(texelSize * -2.0f, 0.0f);
    output.texCoord4 = input.tex + float2(texelSize * -1.0f, 0.0f);
    output.texCoord5 = input.tex + float2(texelSize * 0.0f, 0.0f);
    output.texCoord6 = input.tex + float2(texelSize * 1.0f, 0.0f);
    output.texCoord7 = input.tex + float2(texelSize * 2.0f, 0.0f);
    output.texCoord8 = input.tex + float2(texelSize * 3.0f, 0.0f);
    output.texCoord9 = input.tex + float2(texelSize * 4.0f, 0.0f);
 
    return output;
}
