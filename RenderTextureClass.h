#pragma once

class RenderTextureClass : public AlignedAllocationPolicy<16>
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D11Device*, int, int, float, float);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*);
	void ResetViewports(ID3D11DeviceContext*);
	void SetViewports(ID3D11DeviceContext*, float TopLeftX, float TopLeftY, float TextureRatio);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();

	void UseShaderResourceView(ID3D11DeviceContext*, int);

	void GetProjectionMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	int GetTextureWidth();
	int GetTextureHeight();

private:
	int m_textureWidth, m_textureHeight;
	float m_screenDepth, m_screenNear;
	ID3D11Texture2D* m_renderTargetTexture = nullptr;;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;;
	ID3D11ShaderResourceView* m_shaderResourceView = nullptr;;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	D3D11_VIEWPORT m_viewport;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_orthoMatrix;
};