#pragma once

class TextureShaderClass;
class LightShaderClass;
class BumpMapShaderClass;

class ShaderManagerClass
{
public:
    ShaderManagerClass();
    ShaderManagerClass(const ShaderManagerClass&);
    ~ShaderManagerClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();

    bool RenderTextureShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

    /*
    bool RenderLightShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
        XMFLOAT3, XMFLOAT4, XMFLOAT4, XMFLOAT3, XMFLOAT4, float);
    */

    bool RenderBumpMapShader(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*,
        ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

private:
    TextureShaderClass* m_TextureShader = nullptr;
    LightShaderClass* m_LightShader = nullptr;
    BumpMapShaderClass* m_BumpMapShader = nullptr;
};
