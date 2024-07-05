#pragma once

class DeferredShaderClass
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    struct DeferredBufferType
    {
        XMFLOAT4X4 inverseProjection;
        XMFLOAT4X4 inverseView;
        int useAO;
        int useEnvMap;
    };


public:
    DeferredShaderClass();
    DeferredShaderClass(const DeferredShaderClass&);
    ~DeferredShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, 
        XMMATRIX, XMMATRIX, XMMATRIX,
        XMFLOAT4X4, XMFLOAT4X4, int, int,
        ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, 
        ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
        ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
        ID3D11ShaderResourceView*);

private:
    bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, 
        XMMATRIX, XMMATRIX, XMMATRIX,
        XMFLOAT4X4, XMFLOAT4X4, int, int,
        ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
        ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
        ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
        ID3D11ShaderResourceView*);


    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_layout = nullptr;
    ID3D11Buffer* m_matrixBuffer = nullptr;
    ID3D11Buffer* m_deferredBuffer = nullptr;
};