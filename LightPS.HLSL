// 전역변수

// 텍스쳐
Texture2D ShaderTexture;

// C++ 에서 설정할 수 있음
// 어떻게 텍스쳐의 픽셀이 씌워지는 지 수정함
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
    // 이 텍스쳐 좌표 위치에서 샘플러를 사용하여 텍스쳐에서 픽셀 색깔을 샘플링한다.
    float4 TextureColor = ShaderTexture.Sample(SampleType, input.tex);
    
    // 픽셀의 색상은 ambientColor(주변광) 값보다 어두워 질 수 없다
    float4 color = ambientColor;
    
    // specular color 를 초기화한다.
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // 법선 벡터와 내적을 하기 전 light 벡터의 방향을 반대로 바꾼다.
    float3 lightDir = -lightDirection;
    
    // 뒤집은 lightDirection 벡터와 법선 벡터를 내적하여 빛의 양을 계산한다.
    float lightIntensity = saturate(dot(input.normal, lightDir));

    // 해당 픽셀에 빛이 있으면
    if (lightIntensity > 0.0f)
    {
        // 빛의 양에 따라 최종 diffuseColor 를 결정한다.
        color += (diffuseColor * lightIntensity);
        
        // 최종 빛의 색상을 채운다.
        color = saturate(color);
        
        // 반사 벡터를 계산한다.
        // 반사 벡터 = 2 * 빛의 강도 * 법선 - 빛의 방향
        float3 reflection = normalize(2 * lightIntensity * input.normal - lightDir);
        
        // 반사 벡터, 시선 방향, specularPower 를 기준으로 반사 조명의 양을 결정한다.
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

    }
    
    // 빛의 색과 텍스쳐의 색을 곱해서 이 픽셀의 최종 색상을 결정한다.
    color = color * TextureColor;
    
    // 출력 색상의 마지막에 반사 컴포넌트를 추가한다.
    color = saturate(color + specular);
    
    return color;
}