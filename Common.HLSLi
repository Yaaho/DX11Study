#define BLUE_NOISE_TEXTURE_SIZE 128

float3 ConvertThreadIdToNdc(uint3 id, uint3 dims, float jitter)
{
    float3 ndc = id;
	
	// voxel 의 가운데 지점을 가리키도록 0.5 를 더함
    ndc += 0.5f;
	
	// jitter
	ndc.z += jitter;

    ndc *= float3(2.f / dims.x, -2.f / dims.y, 1.f / dims.z);

	// u 좌표는 -1 ~ 1 로
	// v 좌표는 -1 ~ 1 로
	// w 좌표는 0 ~ 1 로
    ndc += float3(-1.f, 1.f, 0.f);
	
	
    return ndc;
}

float ConvertNdcZToDepth(float ndcZ, float depth_power, float near, float far)
{
    float depthPackExponent = depth_power;
    float nearPlaneDist = near;
    float farPlaneDist = far;

	// 0 ~ 1 인 ndcZ 값을 제곱한 값에
	// 가시거리를 곱해주고
	// nearPlaneDist 값을 더해준다.
    return pow(ndcZ, depthPackExponent) * (farPlaneDist - nearPlaneDist) + nearPlaneDist;
}


float3 ConvertToWorldPosition(float3 ndc, float depth, matrix inv_view, matrix inv_proj)
{
	// view ray 
	// ndc좌표에 대한 카메라 공간 광선을 계산
    float4 viewRay = mul(inv_proj, float4(ndc, 1.f));
    viewRay /= viewRay.w;
    viewRay /= viewRay.z; // z값이 1이 되도록

	// ndc -> world position
	// z 값이 1인 viewRay 에 이전에 구한 depth 값을 곱해주면 z 좌표가 depth 인 뷰 공간에서의 위치가 구해진다.
	// 그 값을 inv_view 에 곱하면 월드 좌표가 된다.
    float4 worldPosition = mul(inv_view, float4(viewRay.xyz * depth, 1.f));
	
	// 이걸 깜빡했다. 절 대 빼먹지 말자
    worldPosition.xyz /= worldPosition.w;

    return worldPosition.xyz;
}


float3 ConvertThreadIdToWorld(uint3 id, uint3 dims, matrix inv_view, matrix inv_proj, float depth_power, float near, float far, float jitter)
{
    float3 ndc = ConvertThreadIdToNdc(id, dims, jitter);
    float depth = ConvertNdcZToDepth(ndc.z, depth_power, near, far);

    return ConvertToWorldPosition(ndc, depth, inv_view, inv_proj);
}


float3 ConvertWorldToNdc(float3 world, matrix vp)
{
    float4 p = mul(float4(world, 1.0f), vp);
        
    if (p.w > 0.0f)
    {
        p.x /= p.w;
        p.y /= p.w;
        p.z /= p.w;
    }
    
    return p.xyz;
}

float3 ConvertWorldToUV(float3 world, matrix view, matrix projection, float depth_power, float near, float far)
{
	matrix viewproj = mul(view, projection);
	
    float3 ndc = ConvertWorldToNdc(world, viewproj);
	
	// -1 ~ 1 의 NDC 범위를 0 -> 1 범위로 변경
    float3 uv = ndc * 0.5f + 0.5f;
	// V 좌표 상하반전
    uv.y = 1 - uv.y;

	// 뷰 포지션
	float4 viewposition = mul(float4(world, 1.0f), view);
	
	// perspective devide
	viewposition.xyz /= viewposition.w;

	// 뷰 포지션에서의 Z 위치
	float ViewZ = viewposition.z;

	// 뷰 좌표의 Z 값을 depth_power 분포에 맞는 UV 좌표로 만듦
	uv.z = pow((ViewZ - near) / (far - near), 1.0f / depth_power);
	
    return uv;
}


// https://gist.github.com/Fewes/59d2c831672040452aa77da6eaab2234
float4 textureTricubic(Texture3D tex, float3 coord, SamplerState samplerState)
{
	// Shift the coordinate from [0,1] to [-0.5, texture_size-0.5]
    float3 texture_size = 0;
	tex.GetDimensions(texture_size.x, texture_size.y, texture_size.z);
	
	float3 coord_grid = coord * texture_size - 0.5;
	float3 index = floor(coord_grid);
	float3 fraction = coord_grid - index;
	float3 one_frac = 1.0 - fraction;

	float3 w0 = 1.0/6.0 * one_frac*one_frac*one_frac;
	float3 w1 = 2.0/3.0 - 0.5 * fraction*fraction*(2.0-fraction);
	float3 w2 = 2.0/3.0 - 0.5 * one_frac*one_frac*(2.0-one_frac);
	float3 w3 = 1.0/6.0 * fraction*fraction*fraction;

	float3 g0 = w0 + w1;
	float3 g1 = w2 + w3;
	float3 mult = 1.0 / texture_size;
	float3 h0 = mult * ((w1 / g0) - 0.5 + index); //h0 = w1/g0 - 1, move from [-0.5, texture_size-0.5] to [0,1]
	float3 h1 = mult * ((w3 / g1) + 1.5 + index); //h1 = w3/g1 + 1, move from [-0.5, texture_size-0.5] to [0,1]

	// Fetch the eight linear interpolations
	// Weighting and fetching is interleaved for performance and stability reasons
	float4 tex000 = tex.SampleLevel(samplerState, h0, 0);
	float4 tex100 = tex.SampleLevel(samplerState, float3(h1.x, h0.y, h0.z), 0);
	tex000 = lerp(tex100, tex000, g0.x); // Weigh along the x-direction

	float4 tex010 = tex.SampleLevel(samplerState, float3(h0.x, h1.y, h0.z), 0);
	float4 tex110 = tex.SampleLevel(samplerState, float3(h1.x, h1.y, h0.z), 0);
	tex010 = lerp(tex110, tex010, g0.x); // Weigh along the x-direction
	tex000 = lerp(tex010, tex000, g0.y); // Weigh along the y-direction

	float4 tex001 = tex.SampleLevel(samplerState, float3(h0.x, h0.y, h1.z), 0);
	float4 tex101 = tex.SampleLevel(samplerState, float3(h1.x, h0.y, h1.z), 0);
	tex001 = lerp(tex101, tex001, g0.x); // Weigh along the x-direction

	float4 tex011 = tex.SampleLevel(samplerState, float3(h0.x, h1.y, h1.z), 0);
	float4 tex111 = tex.SampleLevel(samplerState, float3(h1), 0);
	tex011 = lerp(tex111, tex011, g0.x); // Weigh along the x-direction
	tex001 = lerp(tex011, tex001, g0.y); // Weigh along the y-direction

	return lerp(tex001, tex000, g0.z); // Weigh along the z-direction
}

bool2 GreaterThan(float2 a, float2 b) {
	return step(b, a);
}

bool3 GreaterThan(float3 a, float3 b) {
	return step(b, a);
}

bool4 GreaterThan(float4 a, float4 b) {
	return step(b, a);
}

bool2 LessThan(float2 a, float2 b) {
	return step(a, b);
}

bool3 LessThan(float3 a, float3 b) {
	return step(a, b);
}

bool4 LessThan(float4 a, float4 b) {
	return step(a, b);
}