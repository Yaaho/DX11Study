cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct ConstantOutputType
{
    float edge[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct HullOutputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};



[domain("tri")]

// Domain Shader : 모자이크 처리 된 데이터를 받아서 버텍스 셰이더에서 이전에 사용한 것과 같은 최종 꼭지점을 조작하고 변환하는데 사용됨
PixelInputType ColorDomainShader(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation,
        const OutputPatch<HullOutputType, 3> patch)
{
    float3 vertexPosition;
    PixelInputType output;
    
    // 새로운 정점의 위치를 결정한다.
    vertexPosition = uvwCoord.x * patch[0].position + uvwCoord.y * patch[1].position + uvwCoord.z * patch[2].position;
    
    // 월드 뷰 투영 행렬을 이용하여 새 정점의 위치를 계산한다.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 입력 색상을 픽셀 셰이더로 보낸다.
    output.color = patch[0].color;
    
    return output;
}