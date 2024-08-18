#include "Sampler.HLSLi"

TextureCube SkyboxTexture : register(t0);

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 tex : TEXCOORD0;
};
 
 
float4 SkyboxPixelShader(PixelInputType input) : SV_TARGET
{
    return float4(SkyboxTexture.SampleLevel(LinearSampler, input.tex, 0.0).rgb, 1);
}