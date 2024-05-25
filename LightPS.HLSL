// 전역변수

// 텍스쳐
Texture2D ShaderTexture[3];

// 어떻게 텍스쳐의 픽셀이 씌워지는 지 수정함
// C++ 에서 설정할 수 있음
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 diffuseColor;
    float4 specularColor;
    float3 lightDirection;
    float specularPower;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
    float clip : SV_ClipDistance0;
};

float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    // 이 텍스쳐 좌표 위치에서 샘플러를 사용하여 텍스쳐에서 픽셀 색깔을 샘플링한다.
    float4 textureColor = ShaderTexture[0].Sample(SampleType, input.tex);
    float4 bumpMap = ShaderTexture[1].Sample(SampleType, input.tex);
    
    // 노말 값의 범위를 (0, +1) 에서 (-1, +1)로 확장한다.
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    
    // 범프 맵의 데이터에서 법선을 계산한다.
    float3 bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
    
    // 범프 노말을 노말라이즈 한다.
    bumpNormal = normalize(bumpNormal);
    
    // 픽셀의 색상은 ambientColor(주변광) 값보다 어두워 질 수 없다
    // float4 color = ambientColor;

    // specular color 를 초기화한다.
    // float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // 법선 벡터와 내적을 하기 전 light 벡터의 방향을 반대로 바꾼다.
    float3 lightDir = -lightDirection;
    
    // 뒤집은 lightDirection 벡터와 법선 벡터를 내적하여 빛의 양을 계산한다.
    float lightIntensity = saturate(dot(bumpNormal, lightDir));

    // lightIntensity 를 기반으로 최종 diffuse 색을 결정한다.
    float color = saturate(diffuseColor * lightIntensity);
    
    // 최종 diffuse 색상과 텍스쳐 색상을 결합한다.
    color = color * textureColor;
    
    // 해당 픽셀에 빛이 있으면
    if (lightIntensity > 0.0f)
    {
        // 스패큘러 맵을 가져온다.
        float specularIntensity = ShaderTexture[2].Sample(SampleType, input.tex);
        
        // 반사 벡터를 계산한다.
        // 반사 벡터 = 2 * 빛의 강도 * 법선 - 빛의 방향
        float3 reflection = normalize(2 * lightIntensity * input.normal - lightDir);
        
        // 반사 벡터, 시선 방향, specularPower 를 기준으로 반사 조명의 양을 결정한다.
        float specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
        
        // 스패큘러 맵을 사용하여 이 픽셀의 반사광의 강도를 결정한다.
        specular = specular * specularIntensity;

        // 출력 색상에 최종 정반사 값을 더한다.
        color = saturate(color + specular);
    }
    
    return color;
}