cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer NoiseBuffer
{
    float frameTime;
    float3 scrollSpeeds;
    float3 scales;
    float padding;
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
    float2 texCoords1 : TEXCOORD1;
    float2 texCoords2 : TEXCOORD2;
    float2 texCoords3 : TEXCOORD3;
};

PixelInputType FireVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // 적절한 행렬 계산을 위해 위치 벡터의 4번째 값을 변경하낟.
    input.position.w = 1.0f;
    
    // 월드, 뷰, 투영 행렬에 대한 정점의 위치를 계산한다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 픽셀 셰이더의 텍스쳐 좌표를 계산한다.
    output.tex = input.tex;
    
    // 첫 번째 스케일과 위쪽 스크롤링 속도 값을 사용하여 첫 번째 노이즈 텍스처에 대한 텍스처 좌표를 계산합니다.
    output.texCoords1 = (input.tex * scales.x);
    output.texCoords1.y = output.texCoords1.y + (frameTime * scrollSpeeds.x);
 
    // 두 번째 스케일과 위쪽 스크롤링 속도 값을 사용하여 두 번째 노이즈 텍스처의 텍스처 좌표를 계산합니다.
    output.texCoords2 = (input.tex * scales.y);
    output.texCoords2.y = output.texCoords2.y + (frameTime * scrollSpeeds.y);
 
    // 세 번째 스케일고 위쪽 스크롤링 속도 값을 사용하여 세 번째 노이즈 텍스처의 텍스처 좌표를 계산합니다.
    output.texCoords3 = (input.tex * scales.z);
    output.texCoords3.y = output.texCoords3.y + (frameTime * scrollSpeeds.z);
    
    return output;
}