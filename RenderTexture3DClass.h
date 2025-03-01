#pragma once

class RenderTexture3DClass : public AlignedAllocationPolicy<16>
{
public:
    RenderTexture3DClass();
    RenderTexture3DClass(const RenderTexture3DClass&);
    ~RenderTexture3DClass();

    bool Initialize(ID3D11Device*, int, int, int);
    void Shutdown();

    void SetUnorderedAccessView(ID3D11DeviceContext*);
    ID3D11ShaderResourceView* GetShaderResourceView();
    void UsePSShaderResourceView(ID3D11DeviceContext*, int);
    void UseCSShaderResourceView(ID3D11DeviceContext*, int);

    int GetTextureWidth();
    int GetTextureHeight();
    int GetTextureDepth();

private:
    int m_textureWidth, m_textureHeight, m_textureDepth;

    ID3D11Texture3D* m_texture3D = nullptr;
    ID3D11ShaderResourceView* m_shaderResourceView = nullptr;
    ID3D11UnorderedAccessView* m_unorderedAccessView = nullptr;
};
