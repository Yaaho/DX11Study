Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
};


struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};


float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	//이 텍스처 좌표 위치에서 샘플러를 사용하여 텍스처에서 픽셀 색상을 샘플링합니다.
    float4 textureColor = shaderTexture.Sample(SampleType, input.tex);

	// 모든 픽셀의 기본 출력 색상을 주변 광원 값으로 설정합니다.
    float4 color = ambientColor;

	// specular color를 초기화한다.
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 계산을 위해 빛 방향을 반전시킵니다.
    float3 lightDir = -lightDirection;

    // 이 픽셀의 빛의 양을 계산합니다.
    float lightIntensity = saturate(dot(input.normal, lightDir));

    if (lightIntensity > 0.0f)
    {
        // 확산 색과 광 강도의 양에 따라 최종 확산 색을 결정합니다.
        color += (diffuseColor * lightIntensity);

	    // 주위 및 확산 색상을 포화시킵니다.
        color = saturate(color);

		// 빛의 강도, 법선 벡터 및 빛의 방향에 따라 반사 벡터를 계산합니다.
        float3 reflection = normalize(2 * lightIntensity * input.normal - lightDir);

		// 반사 벡터, 시선 방향 및 반사 출력을 기준으로 반사 조명의 양을 결정합니다.
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    }

	// 텍스처 픽셀과 입력 색상을 곱하여 텍스처 결과를 얻습니다.
    color = color * textureColor;

	// 출력 색상의 마지막에 반사 컴포넌트를 추가합니다.
    color = saturate(color + specular);

    return color;
}