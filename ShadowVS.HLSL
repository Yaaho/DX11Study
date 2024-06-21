
/*
* 그림자 매핑 설명
첫번째 렌더패스:

빛의 시점에서 depth 를 그린 텍스쳐를 만든다.

두번째 렌더패스:

버텍스 셰이더에서 픽셀 셰이더로
카메라의 시점에서 버텍스의 위치,
빛의 시점에서 버텍스의 위치를 넘긴다

픽셀 셰이더에서 빛의 시점에서의 위치를 0, 1의 범위로 바꾼다

그 좌표를 사용하여 depth 를 그린 텍스쳐에서 depth 값을 받아오고 빛의 시점에서 본 현재 픽셀의 깊이와 값을 비교한다.
빛의 시점에서 본 현재 픽셀의 깊이가 더 깊으면 이 픽셀은 그림자 부분이고 ambient light 만 사용하게된다.
텍스쳐에서 얻어온 depth 값이 같거나 더 크면 빛이 비추는 부분이고 ambient light 와 diffuse, specular 를 더한다.
*/

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};
 
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};
 
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
};


PixelInputType ShadowVertexShader(VertexInputType input)
{
    PixelInputType output;
        
    // 적절한 행렬 계산을 위해 위치 벡터를 4 단위로 변경합니다.
    input.position.w = 1.0f;
 
    // 월드, 뷰 및 투영 행렬에 대한 정점의 위치를 계산합니다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 광원에 의해 보았을 때 vertice의 위치를 계산합니다.
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);
 
    // 픽셀 쉐이더의 텍스처 좌표를 저장한다.
    output.tex = input.tex;
    
    // 월드 행렬에 대해서만 법선 벡터를 계산합니다.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    
    // 법선 벡터를 정규화합니다.
    output.normal = normalize(output.normal);
    
    return output;
}