#pragma once

#define mipLevels 6

class RenderTextureCubeClass : public AlignedAllocationPolicy<16>
{
public:
	RenderTextureCubeClass();
	RenderTextureCubeClass(const RenderTextureCubeClass&);
	~RenderTextureCubeClass();

	bool Initialize(ID3D11Device*, int);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*, int);
	void SetViewports(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();

	void UseShaderResourceView(ID3D11DeviceContext*, int);

private:
	int m_textureWidth, m_textureHeight;
	ID3D11Texture2D* m_renderTargetTexture = nullptr;
	ID3D11RenderTargetView* m_renderTargetView[6 * mipLevels];
	ID3D11ShaderResourceView* m_shaderResourceView;

	int m_CubeMapSize;
};