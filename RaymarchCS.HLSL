#include "Common.HLSLi"

#define LOCAL_SIZE_X 8
#define LOCAL_SIZE_Y 8
#define LOCAL_SIZE_Z 1


RWTexture3D<float4> output : register(u0);

SamplerState LinearSampler : register(s0);

cbuffer RaymarchCBuffer : register(b0)
{
    float4 biasNearFarPow;
    float4 anisoDensityScatteringAbsorption;
}

Texture3D voxelGrid : register(t0);


float SliceTickness(float ndcZ, uint dimZ)
{
    return ConvertNdcZToDepth(ndcZ + 1.f / float(dimZ), biasNearFarPow.w, biasNearFarPow.y, biasNearFarPow.z) - ConvertNdcZToDepth(ndcZ, biasNearFarPow.w, biasNearFarPow.y, biasNearFarPow.z);
}


static const float DensityScale = 0.01f;

float4 ScatterStep(float3 accumulatedLight, float accumulatedTransmittance, float3 sliceLight, float sliceDensity, float tickness)
{
    sliceDensity = max(sliceDensity, 0.000001f);
    sliceDensity *= DensityScale;
    float sliceTransmittance = exp(-sliceDensity * tickness);

	// Frostbite의 누적 방식
    float3 sliceLightIntegral = sliceLight * (1.f - sliceTransmittance) / sliceDensity;

    accumulatedLight += sliceLightIntegral * accumulatedTransmittance;
    accumulatedTransmittance *= sliceTransmittance;

    return float4(accumulatedLight, accumulatedTransmittance);
}



[numthreads(LOCAL_SIZE_X, LOCAL_SIZE_Y, LOCAL_SIZE_Z)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint3 coord = DTid.xyz;
    int3 max;
    voxelGrid.GetDimensions(max.x, max.y, max.z);
    
    if (all(coord < max))
    {
        float4 accum = float4(0.0f, 0.0f, 0.0f, 1.0f);
        uint3 pos = uint3(DTid.xy, 0);
        
        for (int z = 0; z < max.z; z++)
        {
            pos.z = z;
            float4 slice = voxelGrid[pos];
            
            float tickness = SliceTickness((float) z / max.z, max.z);
            accum = ScatterStep(accum.rgb, accum.a, slice.rgb, slice.a, tickness);

            output[pos] = accum;
        }
    }
}