#include "Stdafx.h"
#include "LightInjectionShaderClass.h"


LightInjectionShaderClass::LightInjectionShaderClass()
{
}

LightInjectionShaderClass::LightInjectionShaderClass(const LightInjectionShaderClass& other)
{
    
}

LightInjectionShaderClass::~LightInjectionShaderClass()
{
    
}

bool LightInjectionShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    // 컴퓨트 쉐이더를 초기화한다.
    return InitializeShader(device, hwnd, L"LightInjectionCS.HLSL");
}

void LightInjectionShaderClass::Shutdown()
{
    // 컴퓨트 쉐이더와 관련된 객체를 종료한다.
    ShutdownShader();
}

void LightInjectionShaderClass::UsePSVolumetricBuffer(ID3D11DeviceContext* deviceContext, int slot)
{
    deviceContext->PSSetConstantBuffers(slot, 1, &m_lightInjectionBuffer);
}

void LightInjectionShaderClass::UseCSVolumetricBuffer(ID3D11DeviceContext* deviceContext, int slot)
{
    deviceContext->CSSetConstantBuffers(slot, 1, &m_lightInjectionBuffer);
}

bool LightInjectionShaderClass::Compute(ID3D11DeviceContext* deviceContext,
    XMMATRIX invView, XMMATRIX invProjection,
    XMFLOAT4 cameraPosition, XMFLOAT4 biasNearFarPow, XMFLOAT4 anisoDensityScatteringAbsorption,
    ID3D11ShaderResourceView* blueNoises, int accumulation,
    int width, int height, int depth)
{
    // 사용할 셰이더 매개 변수를 설정합니다.
    if (!SetShaderParameters(deviceContext,
        invView, invProjection,
        cameraPosition, biasNearFarPow, anisoDensityScatteringAbsorption, blueNoises, accumulation))
    {
        return false;
    }

    // 설정된 버퍼를 셰이더로 렌더링한다.
    ComputeShader(deviceContext, width, height, depth);

    return true;
}

bool LightInjectionShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* csFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    // 컴퓨트 셰이더 코드를 컴파일한다.
    ID3D10Blob* computeShaderBuffer = nullptr;
    // 세이더 파일 내에 include 가 있으므로 D3D_COMPILE_STANDARD_FILE_INCLUDE 를 사용해야 한다.
    result = D3DCompileFromFile(csFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &computeShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // 셰이더 컴파일 실패시 오류메시지를 출력합니다.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, csFilename);
        }
        // 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
        else
        {
            MessageBox(hwnd, csFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // 버퍼로부터 컴퓨트 셰이더를 생성한다.
    result = device->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), NULL, &m_computeShader);
    if (FAILED(result))
    {
        return false;
    }

    computeShaderBuffer->Release();
    computeShaderBuffer = 0;

    D3D11_BUFFER_DESC lightInjectionBufferDesc;
    lightInjectionBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    lightInjectionBufferDesc.ByteWidth = sizeof(LightInjectionCBufferType);
    lightInjectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightInjectionBufferDesc.CPUAccessFlags = 0;
    lightInjectionBufferDesc.MiscFlags = 0;
    lightInjectionBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&lightInjectionBufferDesc, NULL, &m_lightInjectionBuffer);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void LightInjectionShaderClass::ShutdownShader()
{
    if (m_lightInjectionBuffer)
    {
        m_lightInjectionBuffer->Release();
        m_lightInjectionBuffer = 0;
    }
    
    if (m_computeShader)
    {
        m_computeShader->Release();
        m_computeShader = 0;
    }
}

void LightInjectionShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    // 에러 메시지를 출력창에 표시합니다.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // 에러 메세지를 반환합니다.
    errorMessage->Release();
    errorMessage = 0;

    // 컴파일 에러가 있음을 팝업 메세지로 알려줍니다.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool LightInjectionShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
    XMMATRIX invView, XMMATRIX invProjection,
    XMFLOAT4 cameraPosition, XMFLOAT4 biasNearFarPow, XMFLOAT4 anisoDensityScatteringAbsorption, ID3D11ShaderResourceView* blueNoises, int accumulation)
{
    // prevView, prevProjection 은 이미 설정되어 있다.
    m_VolumetricProps.invView = invView;
    m_VolumetricProps.invProjection = invProjection;
    m_VolumetricProps.cameraPosition = cameraPosition;
    m_VolumetricProps.biasNearFarPow = biasNearFarPow;
    m_VolumetricProps.anisoDensityScatteringAbsorption = anisoDensityScatteringAbsorption;
    m_VolumetricProps.accumulation = accumulation;
    m_VolumetricProps.padding0 = 0;
    m_VolumetricProps.padding1 = 0;
    m_VolumetricProps.padding2 = 0;

    deviceContext->UpdateSubresource(m_lightInjectionBuffer, 0, nullptr, &m_VolumetricProps, 0, 0);

    // 컴퓨트 셰이더에서의 상수 버퍼의 위치를 설정한다.
    // b0
    unsigned int bufferNumber = 0;
    deviceContext->CSSetConstantBuffers(bufferNumber, 1, &m_lightInjectionBuffer);
    
    // t2
    // 컴퓨트 셰이더에서의 셰이더 리소스 위치를 설정한다.
    bufferNumber = 2;
    deviceContext->CSSetShaderResources(bufferNumber, 1, &blueNoises);

    return true;
}

void LightInjectionShaderClass::ComputeShader(ID3D11DeviceContext* deviceContext, int width, int height, int depth)
{
    deviceContext->CSSetShader(m_computeShader, NULL, 0);

    deviceContext->Dispatch(width, height, depth);
}





