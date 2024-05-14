
cbuffer PerFrameBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
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
};
 
PixelInputType FontVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // 좌표계 변환을 위해 위치 벡터를 4 단위로 변경함.
    input.position.w = 1.0f;
 
    // 월드, 뷰, 투영 행렬로 정점의 위치를 계산함.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // 픽셀 셰이더의 텍스쳐 좌표를 저장함.
    output.tex = input.tex;
    
    return output;
}