cbuffer MatrixBuffer
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

PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // 트랜스폼 변환 행렬 계산을 위해 위치 벡터를 4 단위로 변환한다.
    input.position.w = 1.0f;
    
    // 월드, 뷰, 투영 행렬에 대한 정점의 위치를 계산한다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 버텍스의 텍스쳐 좌표를 픽셀 쉐이더의 input으로 넣어 좌표 사이가 보간된다.
    output.tex = input.tex;
    
    return output;
}
    




