struct VertexInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct HullInputType
{
    float3 position : POSITION;
    float4 color : COLOR;
};

HullInputType ColorVertexShader(VertexInputType input)
{
    HullInputType output;
    
    // 정점 위치를 선체 쉐이더에 전달한다.
    output.position = input.position;
    output.color = input.color;
    
    return output;
}