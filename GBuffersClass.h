#pragma once

const int BUFFER_COUNT = 4;

class GBuffersClass
{
public:
	GBuffersClass();
	GBuffersClass(const GBuffersClass&);
	~GBuffersClass();

	bool Initialize(ID3D11Device*, int, int, float, float);
	void Shutdown();

	void SetRenderTargets(ID3D11DeviceContext*);
	void ClearRenderTargets(ID3D11DeviceContext*, float, float, float, float);

	ID3D11DepthStencilView* GetDepthStencilView();
	ID3D11ShaderResourceView* GetShaderResourceView(int);
	ID3D11ShaderResourceView* GetDepthResourceView();

private:
	int m_textureWidth = 0;
	int m_textureHeight = 0;
	ID3D11Texture2D* m_renderTargetTextureArray[BUFFER_COUNT] = { 0, };
	ID3D11RenderTargetView* m_renderTargetViewArray[BUFFER_COUNT] = { 0, };
	ID3D11ShaderResourceView* m_shaderResourceViewArray[BUFFER_COUNT] = { 0, };
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11ShaderResourceView* m_depthResourceView = nullptr;
	D3D11_VIEWPORT m_viewport;

};