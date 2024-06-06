Texture2D fireTexture : register(t0);
Texture2D noiseTexture : register(t1);
Texture2D alphaTexture : register(t2);
SamplerState SampleType;
SamplerState SampleType2;

cbuffer DistortionBuffer
{
    float2 distortion1;
    float2 distortion2;
    float2 distortion3;
    float distortionScale;
    float distortionBias;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float2 texCoords1 : TEXCOORD1;
    float2 texCoords2 : TEXCOORD2;
    float2 texCoords3 : TEXCOORD3;
};

float4 FirePixelShader(PixelInputType input) : SV_TARGET
{
    float4 finalNoise;
    float perturb;
    float2 noiseCoords;
    float4 fireColor;
    float4 alphaColor;
    
    // 서로 다른 3개의 texCoord 를 사용하여 노이즈 텍스쳐를 샘플링한다.
    // texCoord 는 노이즈 스케일과 스크롤 속도에서 차이가 있다
    float4 noise1 = noiseTexture.Sample(SampleType, input.texCoords1);
    float4 noise2 = noiseTexture.Sample(SampleType, input.texCoords2);
    float4 noise3 = noiseTexture.Sample(SampleType, input.texCoords3);
    
    // (0, 1) 범위에서 (-1, +1) 범위로 노이즈 좌표를 이동한다.
    noise1 = (noise1 - 0.5f) * 2.0f;
    noise2 = (noise2 - 0.5f) * 2.0f;
    noise3 = (noise3 - 0.5f) * 2.0f;
    
    // 서로 다른 왜곡값으로 노이즈의 좌표를 왜곡한다.
    noise1.xy = noise1.xy * distortion1.xy;
    noise2.xy = noise2.xy * distortion2.xy;
    noise3.xy = noise3.xy * distortion3.xy;
    
    // 세가지 왜곡된 노이즈 결과를 하나의 노이즈 결과로 결합한다.
    finalNoise = noise1 + noise2 + noise3;
    
    // 왜곡 스케일과 왜곡 바이어스 값에 의해 입력 텍스쳐 y 좌표 결과를 왜곡한다.
    perturb = ((1.0f - input.tex.y) * distortionScale) + distortionBias;
    
    // 왜곡된 텍스쳐 샘플링 좌표를 만든다.
    noiseCoords.xy = (finalNoise.xy * perturb) + input.tex.xy;
    
    // 왜곡된 텍스쳐 샘플링 좌표로 fireTexture 를 샘플링한다.
    // SampleType2 는 샘플링에 Clamp 방법을 사용하여 화염의 아래 부분에 위의 텍스쳐가 사용되는 일이 없도록 한다.
    fireColor = fireTexture.Sample(SampleType2, noiseCoords.xy);
    
    // 왜곡된 텍스쳐 샘플링 좌표로 alphaTexture 를 샘플링한다.
    // 이것은 불의 투명도에 사용된다.
    alphaColor = alphaTexture.Sample(SampleType2, noiseCoords.xy);
    
    // 화염의 알파 블렌딩을 왜곡된 알파 텍스쳐 값으로 설정한다.
    fireColor.a = alphaColor;
    
    return fireColor;
}