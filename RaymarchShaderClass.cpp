#include "Stdafx.h"
#include "RaymarchShaderClass.h"


RaymarchShaderClass::RaymarchShaderClass()
{
}

RaymarchShaderClass::RaymarchShaderClass(const RaymarchShaderClass& other)
{
    
}

RaymarchShaderClass::~RaymarchShaderClass()
{
    
}

bool RaymarchShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    // 컴퓨트 쉐이더를 초기화한다.
    return InitializeShader(device, hwnd, L"RaymarchCS.HLSL");
}

void RaymarchShaderClass::Shutdown()
{
    // 컴퓨트 쉐이더와 관련된 객체를 종료한다.
    ShutdownShader();
}

bool RaymarchShaderClass::Compute(ID3D11DeviceContext* deviceContext, XMFLOAT4 biasNearFarPow, XMFLOAT4 anisoDensityScatteringAbsorption, int width, int height, int depth)
{
    // 사용할 셰이더 매개 변수를 설정합니다.
    if (!SetShaderParameters(deviceContext, biasNearFarPow, anisoDensityScatteringAbsorption))
    {
        return false;
    }

    // 설정된 버퍼를 셰이더로 렌더링한다.
    ComputeShader(deviceContext, width, height, depth);

    return true;
}

bool RaymarchShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* csFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    // 컴퓨트 셰이더 코드를 컴파일한다.
    ID3D10Blob* computeShaderBuffer = nullptr;
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

    D3D11_BUFFER_DESC raymarchBufferDesc;
    raymarchBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    raymarchBufferDesc.ByteWidth = sizeof(RaymarchCBufferType);
    raymarchBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    raymarchBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    raymarchBufferDesc.MiscFlags = 0;
    raymarchBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&raymarchBufferDesc, NULL, &m_raymarchBuffer);
    if (FAILED(result))
    {
        return false;
    }
    
    return true;
}


void RaymarchShaderClass::ShutdownShader()
{
    if (m_raymarchBuffer)
    {
        m_raymarchBuffer->Release();
        m_raymarchBuffer = 0;
    }
    
    if (m_computeShader)
    {
        m_computeShader->Release();
        m_computeShader = 0;
    }
}

void RaymarchShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    // 에러 메시지를 출력창에 표시합니다.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // 에러 메세지를 반환합니다.
    errorMessage->Release();
    errorMessage = 0;

    // 컴파일 에러가 있음을 팝업 메세지로 알려줍니다.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}


bool RaymarchShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4 biasNearFarPow, XMFLOAT4 anisoDensityScatteringAbsorption)
{
    // 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    if (FAILED(deviceContext->Map(m_raymarchBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    RaymarchCBufferType* dataptr = (RaymarchCBufferType*)mappedResource.pData;
    dataptr->biasNearFarPow = biasNearFarPow;
    dataptr->anisoDensityScatteringAbsorption = anisoDensityScatteringAbsorption;

    deviceContext->Unmap(m_raymarchBuffer, 0);

    // b0
    unsigned int bufferNumber = 0;
    deviceContext->CSSetConstantBuffers(bufferNumber, 1, &m_raymarchBuffer);

    return true;
}

void RaymarchShaderClass::ComputeShader(ID3D11DeviceContext* deviceContext, int width, int height, int depth)
{
    deviceContext->CSSetShader(m_computeShader, NULL, 0);

    deviceContext->Dispatch(width, height, depth);
}


