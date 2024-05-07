
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    // SV_POSITION의 SV는 SystemValue의 약자입니다.
    // HLSL상에서 이미 예약되어 있는 이름이고
    // 렌더링 파이프라인 상에서의 의미가 정해져 있다.
    
    float4 postion : SV_Position;
    float4 color : COLOR;
};


PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // 트랜스폼 번환 행렬 계산을 위해 위치 벡터를 4단위로 변경한다.
    input.position.w = 1.0f;
    
    // 월드, 뷰, 및 투영 행렬에 대한 정점의 위치를 계산한다.
    output.postion = mul(input.position, worldMatrix);
    output.postion = mul(output.postion, viewMatrix);
    output.postion = mul(output.postion, projectionMatrix);
    
    // 픽셀 쉐이더가 사용할 입력 색상을 지정한다.
    output.color = input.color;
    
    return output;
}





