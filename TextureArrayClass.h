#pragma once


class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();

	bool Initialize(ID3D11Device*, const WCHAR*);
	bool Initialize(ID3D11Device*, const WCHAR*, const WCHAR*);
	bool Initialize(ID3D11Device*, const WCHAR*, const WCHAR*, const WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture(int index);
	ID3D11ShaderResourceView** GetTextureArray();

private:
	ID3D11ShaderResourceView* m_texture[3] = { nullptr, nullptr, nullptr };

	int textureCount;
};