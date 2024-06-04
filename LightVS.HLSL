
#define NUM_LIGHTS 4

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
    float3 normal : NORMAL;
};

cbuffer LightPositionBuffer
{
    float4 lightPosition[NUM_LIGHTS];
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 lightPos1 : TEXCOORD1;
    float3 lightPos2 : TEXCOORD2;
    float3 lightPos3 : TEXCOORD3;
    float3 lightPos4 : TEXCOORD4;
};

PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;

    // 적절한 행렬 계산을 위해 위치 벡터를 4단위로 설정한다.
    input.position.w = 1.0f;

    // 월드, 뷰, 투영 행렬에 대한 정점의 위치를 계산한다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // 픽셀 셰이더의 텍스쳐 좌표를 저장한다.
    output.tex = input.tex;
    
    // 월드 행렬에 대해서만 법선 벡터를 계산한다.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
	
    // 법선 벡터를 정규화한다.
    output.normal = normalize(output.normal);
	
    // 정점의 월드 위치를 계산
    float4 worldPosition = mul(input.position, worldMatrix);
    
    // 조명의 위치와 정점의 월드 위치를 기반으로 조명의 벡터를 결정한다.
    output.lightPos1.xyz = lightPosition[0].xyz - worldPosition.xyz;
    output.lightPos2.xyz = lightPosition[1].xyz - worldPosition.xyz;
    output.lightPos3.xyz = lightPosition[2].xyz - worldPosition.xyz;
    output.lightPos4.xyz = lightPosition[3].xyz - worldPosition.xyz;
    
    // 조명의 벡터를 노말라이즈 한다.
    output.lightPos1 = normalize(output.lightPos1);
    output.lightPos2 = normalize(output.lightPos2);
    output.lightPos3 = normalize(output.lightPos3);
    output.lightPos4 = normalize(output.lightPos4);
    
    return output;
}