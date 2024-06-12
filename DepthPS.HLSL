struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXTURE0;
};


float4 DepthPixelShader(PixelInputType input) : SV_TARGET
{
    float depthValue;
    float4 color;
	
	// 	Z 픽셀 깊이를 균질 W 좌표로 나누어 픽셀의 깊이 값을 가져옵니다.
    depthValue = input.depthPosition.z / input.depthPosition.w;

    color = float4(depthValue, depthValue, depthValue, 1.0f);

    return color;
}