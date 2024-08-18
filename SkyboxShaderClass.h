#pragma once


class SkyboxShaderClass : public AlignedAllocationPolicy<16>
{
private:
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

    struct SpecularBufferType
    {
        float roughness;
        float padding1;
        float padding2;
        float padding3;
    };

public:
    SkyboxShaderClass();
    SkyboxShaderClass(const SkyboxShaderClass&);
    ~SkyboxShaderClass();


    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();

    bool RenderSkybox(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
    bool RenderEnvMap(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
    bool RenderEnvPreFilter(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, float);
    bool RenderBrdfLUT(ID3D11DeviceContext*);

private:
    bool InitializeShader(ID3D11Device*, HWND, 
        const WCHAR*, const WCHAR*,
        const WCHAR*, const WCHAR*,
        const WCHAR*, const WCHAR*);

    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

    bool SetSkyboxShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
    bool SetEnvMapShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);
    bool SetEnvPreFilterShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*, float);

    void RenderSkyboxShader(ID3D11DeviceContext*, int);
    void RenderEnvMapShader(ID3D11DeviceContext*, int);
    void RenderEnvPreFilterShader(ID3D11DeviceContext*, int);
    void RenderBrdfLUTShader(ID3D11DeviceContext*);


private:
    ID3D11VertexShader* m_skyboxVertexShader = nullptr;
    ID3D11VertexShader* m_fullscreenVertexShader = nullptr;

    ID3D11PixelShader* m_skyboxPixelShader = nullptr;
    ID3D11PixelShader* m_envMapPixelShader = nullptr;
    ID3D11PixelShader* m_envPreFilterPixelShader = nullptr;
    ID3D11PixelShader* m_brdfLUTPixelShader = nullptr;

    ID3D11InputLayout* m_skyboxVertexLayout = nullptr;
    ID3D11InputLayout* m_fullscreenVertexLayout = nullptr;

    ID3D11Buffer* m_matrixBuffer = nullptr;
    ID3D11Buffer* m_specularBuffer = nullptr;
};