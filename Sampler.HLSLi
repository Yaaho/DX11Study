#ifndef SAMPLER_COMMON
#define SAMPLER_COMMON

SamplerState LinearSampler : register(s0);
SamplerState PointSampler : register(s1);
SamplerComparisonState CompSampler : register(s2);

#endif