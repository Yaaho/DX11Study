cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};
 

struct VertexInputType
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};
 
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 pos : POSITION0;
    float4 wPosition : POSITION1;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};
 

PixelInputType GBufferVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;
    
    // 픽셀 셰이더로 나갈 wPosition 구함
    output.wPosition = mul(input.position, worldMatrix);
    
    // 픽셀 셰이더로 나갈 position 구함
    output.position = mul(output.wPosition, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 픽셀 셰이더로 나갈 pos 구함
    output.pos = output.position;
    
    // 픽셀 셰이더로 나갈 tex 구함
    output.texCoord = input.texCoord;
    
    
    // 픽셀 셰이더로 나갈 노말, 탄젠트, 바이노말 구함.
    output.normal = normalize(mul(input.normal, (float3x3) worldMatrix));
    output.tangent = normalize(mul(input.tangent, (float3x3) worldMatrix));
    output.binormal = normalize(mul(input.binormal, (float3x3) worldMatrix));
 
    return output;
}