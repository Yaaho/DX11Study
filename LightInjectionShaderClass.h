#pragma once


class LightInjectionShaderClass
{
private:
    struct LightInjectionCBufferType
    {
        XMMATRIX prevView;
        XMMATRIX prevProjection;
        XMMATRIX invView;
        XMMATRIX invProjection;
        XMFLOAT4 cameraPosition;
        XMFLOAT4 biasNearFarPow;
        XMFLOAT4 anisoDensityScatteringAbsorption;
        int accumulation;
        int padding0;
        int padding1;
        int padding2;
    };
    
public:
    LightInjectionShaderClass();
    LightInjectionShaderClass(const LightInjectionShaderClass&);
    ~LightInjectionShaderClass();
    
    bool Initialize(ID3D11Device*, HWND);
    void Shutdown();

    void UsePSVolumetricBuffer(ID3D11DeviceContext*, int);
    void UseCSVolumetricBuffer(ID3D11DeviceContext*, int);
    
    bool Compute(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4, XMFLOAT4, ID3D11ShaderResourceView*, int, int, int, int);

private:
    bool InitializeShader(ID3D11Device*, HWND, const WCHAR*);
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

    bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4, XMFLOAT4, ID3D11ShaderResourceView*, int);

    void ComputeShader(ID3D11DeviceContext*, int, int, int);

private:
    ID3D11ComputeShader* m_computeShader = nullptr;
    ID3D11Buffer* m_lightInjectionBuffer = nullptr;

public:
    LightInjectionCBufferType m_VolumetricProps;
};
