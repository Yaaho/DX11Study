#pragma once

class GBufferShaderClass
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    struct PBRMaterialBufferType
    {
        XMFLOAT4 gAlbedo;
        float gMetallic;
        float gRoughness;

        int gUseAlbedoMap;
        int gUseOccMetalRough;
        int gUseAoMap;
        int gUseEmmisive;
        int gNormalState;
        int gConvertToLinear;
    };

public:
    GBufferShaderClass();
    GBufferShaderClass(const GBufferShaderClass&);
    ~GBufferShaderClass();

    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, 
        ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
        XMFLOAT4, float, float, int, int, int, int, int, int);

private:
    bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, 
        ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
        XMFLOAT4, float, float, int, int, int, int, int, int);
    void RenderShader(ID3D11DeviceContext*, int);

private:
    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_layout = nullptr;
    ID3D11Buffer* m_matrixBuffer = nullptr;
    ID3D11Buffer* m_PBRMaterialBuffer = nullptr;
    ID3D11SamplerState* m_LinearSamplerState = nullptr;
    ID3D11SamplerState* m_PointSamplerState = nullptr;
};