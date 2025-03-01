#pragma once


class SamplerClass
{
public:
	SamplerClass();
	SamplerClass(const SamplerClass&);
	~SamplerClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();

	void UsePSLinear(ID3D11DeviceContext*, int);
	void UseCSLinear(ID3D11DeviceContext*, int);


	void UsePSPoint(ID3D11DeviceContext*, int);
	void UseCSPoint(ID3D11DeviceContext*, int);

	void UsePSComp(ID3D11DeviceContext*, int);
	void UseCSComp(ID3D11DeviceContext*, int);


	ID3D11SamplerState* m_LinearSamplerState = nullptr;
	ID3D11SamplerState* m_PointSamplerState = nullptr;
	ID3D11SamplerState* m_ComparisonSamplerState = nullptr;
};