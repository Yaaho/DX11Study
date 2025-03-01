#pragma once

class RaymarchShaderClass
{
private:
    struct RaymarchCBufferType
    {
        XMFLOAT4 biasNearFarPow;
        XMFLOAT4 anisoDensityScatteringAbsorption;
    };
    
public:
    RaymarchShaderClass();
    RaymarchShaderClass(const RaymarchShaderClass&);
    ~RaymarchShaderClass();
    
    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();
    bool Compute(ID3D11DeviceContext*, XMFLOAT4, XMFLOAT4, int, int, int);

private:
    bool InitializeShader(ID3D11Device*, HWND, const WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, XMFLOAT4, XMFLOAT4);

    void ComputeShader(ID3D11DeviceContext*, int, int, int);

private:
    ID3D11ComputeShader* m_computeShader = nullptr;
    ID3D11Buffer* m_raymarchBuffer = nullptr;
};