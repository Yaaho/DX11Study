SamplerState SampleType;
Texture2D reflectionTexture;
Texture2D refractionTexture;
Texture2D normalTexture;

cbuffer WaterBuffer
{
    float waterTranslation;
    float reflectRefractScale;
    float2 padding;
};


struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
    float4 refractionPosition : TEXCOORD2;
};


float4 WaterPixelShader(PixelInputType input) : SV_TARGET
{
    float2 reflectTexCoord;
    float2 refractTexCoord;
    float4 normalMap;
    float3 normal;
    float4 reflectionColor;
    float4 refractionColor;
    float4 color;
    
    // 움직이는 물을 시뮬레이트하기 위해 물 수직선을 샘플링 한 위치를 이동한다
    input.tex.y += waterTranslation;
    
    // 투사 된 반사 텍스쳐 좌표를 계산한다.
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;
    
    // 투영 된 굴절 텍스쳐 좌표를 계산한다.
    refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / 2.0f + 0.5f;
    refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / 2.0f + 0.5f;
    
    // 노멀 맵 텍스쳐로부터 법선을 샘플링한다.
    normalMap = normalTexture.Sample(SampleType, input.tex);
    
    // 법선의 범위를 (0, 1) 에서 (-1, +1) 로 확장한다.
    normal = (normalMap.xyz * 2.0f) - 1.0f;
    
    // 텍스쳐 좌표 샘플링 위치를 노멀 맵으로 재배치하여 파동 파 효과를 시뮬레이트 한다.
    reflectTexCoord = reflectTexCoord + (normal.xy * reflectRefractScale);
    refractTexCoord = refractTexCoord + (normal.xy * reflectRefractScale);
    
    // 업데이트 된 텍스쳐 좌표를 사용하여 텍스쳐를 샘플링한다.
    reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);
    refractionColor = refractionTexture.Sample(SampleType, refractTexCoord);
    
    // 최종 색상의 반사 및 굴절 결과를 결합한다.
    color = lerp(reflectionColor, refractionColor, 0.6f);
    
    return color;
}


