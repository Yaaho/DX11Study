#pragma once


class SamplerClass
{
public:
	SamplerClass();
	SamplerClass(const SamplerClass&);
	~SamplerClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();

	void UseLinear(ID3D11DeviceContext*, int);
	void UsePoint(ID3D11DeviceContext*, int);

	ID3D11SamplerState* m_LinearSamplerState = nullptr;
	ID3D11SamplerState* m_PointSamplerState = nullptr;
};