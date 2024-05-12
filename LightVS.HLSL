// constant buffer 는 CPU 액세스 대기 시간이 짧고 CPU 에서 자주 업데이트 되는 변수에 사용한다.

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
};

// 버텍스에 담겨져 있는 정보
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

// 픽셀 셰이더에 넘겨주는 정보
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output = (PixelInputType) 0;
    
    // 트랜스폼 변환 행렬 계산을 위해 위치 벡터를 4 단위로 변환한다.
    input.position.w = 1.0f;
    
    // 월드, 뷰, 투영 행렬에 대한 정점의 위치를 계산한다.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    
    // 버텍스의 텍스쳐 좌표를 픽셀 쉐이더의 input으로 넣어 좌표 사이가 보간된다.
    output.tex = input.tex;
    
    
    // 법선 벡터를 월드 좌표계로 만들기 위해 worldMatrix를 이용해 tranform 한다.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    
    // 법선 벡터를 노말라이즈 한다.
    output.normal = normalize(output.normal);
    
    
    // 이 버텍스의 월드 위치를 구한다.
    float4 WorldPosition = mul(input.position, worldMatrix);
    
    // 카메라가 이 버텍스를 보는 방향의 벡터를 구한다.
    output.viewDirection = cameraPosition.xyz - WorldPosition.xyz;
    
    // viewDirection 벡터를 노말라이즈 한다.
    output.viewDirection = normalize(output.viewDirection);
    
    return output;
}
    


