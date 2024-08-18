#include "Stdafx.h"
#include "SkyboxShaderClass.h"


SkyboxShaderClass::SkyboxShaderClass()
{

}

SkyboxShaderClass::SkyboxShaderClass(const SkyboxShaderClass& orther)
{

}

SkyboxShaderClass::~SkyboxShaderClass()
{

}


bool SkyboxShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    return InitializeShader(device, hwnd,
        L"SkyboxVS.HLSL", L"SkyboxPS.HLSL",
        L"IrradianceMapPS.HLSL", L"SpecularPreFilterPS.HLSL",
        L"FullscreenVS.HLSL", L"IntegrateBRDFPS.HLSL");
}

void SkyboxShaderClass::Shutdown()
{
    // 버텍스 및 픽셀 쉐이더와 관련된 객체를 종료합니다.
    ShutdownShader();
}

bool SkyboxShaderClass::RenderSkybox(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* SkyboxTexture)
{
    if (!SetSkyboxShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, SkyboxTexture))
    {
        return false;
    }

    RenderSkyboxShader(deviceContext, indexCount);

    return true;
}


bool SkyboxShaderClass::RenderEnvMap(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* CubeMapTexture)
{
    if (!SetEnvMapShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, CubeMapTexture))
    {
        return false;
    }

    RenderEnvMapShader(deviceContext, indexCount);

    return true;
}


bool SkyboxShaderClass::RenderEnvPreFilter(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* SkyboxTexture, float roughness)
{
    if (!SetEnvPreFilterShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, SkyboxTexture, roughness))
    {
        return false;
    }

    RenderEnvPreFilterShader(deviceContext, indexCount);

    return true;
}


bool SkyboxShaderClass::RenderBrdfLUT(ID3D11DeviceContext* deviceContext)
{
    RenderBrdfLUTShader(deviceContext);

    return true;
}


bool SkyboxShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd,
    const WCHAR* vsSkyboxFilename, const WCHAR* psSkyboxFilename,
    const WCHAR* psEnvMapFilename, const WCHAR* psPreFilterFilename,
    const WCHAR* vsFullscreenFilename, const WCHAR* psBrdfLUTFilename)
{
    ID3D10Blob* errorMessage = nullptr;
    
    ID3D10Blob* skyboxVertexShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFile(vsSkyboxFilename, NULL, NULL, "SkyboxVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &skyboxVertexShaderBuffer, &errorMessage)))
    {
        // 셰이더 컴파일 실패시 오류메시지를 출력합니다.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsSkyboxFilename);
        }
        // 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
        else
        {
            MessageBox(hwnd, vsSkyboxFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    ID3D10Blob* skyboxPixelShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFile(psSkyboxFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "SkyboxPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &skyboxPixelShaderBuffer, &errorMessage)))
    {
        // 셰이더 컴파일 실패시 오류메시지를 출력합니다.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psSkyboxFilename);
        }
        // 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
        else
        {
            MessageBox(hwnd, psSkyboxFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    ID3D10Blob* envMapPixelShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFile(psEnvMapFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "IrradianceMapPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &envMapPixelShaderBuffer, &errorMessage)))
    {
        // 셰이더 컴파일 실패시 오류메시지를 출력합니다.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psEnvMapFilename);
        }
        // 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
        else
        {
            MessageBox(hwnd, psEnvMapFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    ID3D10Blob* preFilterPixelShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFile(psPreFilterFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "SpecularPreFilterPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &preFilterPixelShaderBuffer, &errorMessage)))
    {
        // 셰이더 컴파일 실패시 오류메시지를 출력합니다.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psPreFilterFilename);
        }
        // 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
        else
        {
            MessageBox(hwnd, psPreFilterFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    ID3D10Blob* fullscreenVertexShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFile(vsFullscreenFilename, NULL, NULL, "FullscreenVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &fullscreenVertexShaderBuffer, &errorMessage)))
    {
        // 셰이더 컴파일 실패시 오류메시지를 출력합니다.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFullscreenFilename);
        }
        // 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
        else
        {
            MessageBox(hwnd, vsFullscreenFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    ID3D10Blob* brdfLUTPixelShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFile(psBrdfLUTFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "IntegrateBRDFPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
        &brdfLUTPixelShaderBuffer, &errorMessage)))
    {
        // 셰이더 컴파일 실패시 오류메시지를 출력합니다.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psBrdfLUTFilename);
        }
        // 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
        else
        {
            MessageBox(hwnd, psBrdfLUTFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }



    // 버퍼로부터 정점 셰이더를 생성한다.
    if (FAILED(device->CreateVertexShader(skyboxVertexShaderBuffer->GetBufferPointer(), skyboxVertexShaderBuffer->GetBufferSize(), NULL,
        &m_skyboxVertexShader)))
    {
        return false;
    }

    // 버퍼로부터 픽셀 셰이더를 생성한다.
    if (FAILED(device->CreatePixelShader(skyboxPixelShaderBuffer->GetBufferPointer(), skyboxPixelShaderBuffer->GetBufferSize(), NULL,
        &m_skyboxPixelShader)))
    {
        return false;
    }

    // 버퍼로부터 픽셀 셰이더를 생성한다.
    if (FAILED(device->CreatePixelShader(envMapPixelShaderBuffer->GetBufferPointer(), envMapPixelShaderBuffer->GetBufferSize(), NULL,
        &m_envMapPixelShader)))
    {
        return false;
    }

    // 버퍼로부터 픽셀 셰이더를 생성한다.
    if (FAILED(device->CreatePixelShader(preFilterPixelShaderBuffer->GetBufferPointer(), preFilterPixelShaderBuffer->GetBufferSize(), NULL,
        &m_envPreFilterPixelShader)))
    {
        return false;
    }

    if (FAILED(device->CreateVertexShader(fullscreenVertexShaderBuffer->GetBufferPointer(), fullscreenVertexShaderBuffer->GetBufferSize(), NULL,
        &m_fullscreenVertexShader)))
    {
        return false;
    }

    if (FAILED(device->CreatePixelShader(brdfLUTPixelShaderBuffer->GetBufferPointer(), brdfLUTPixelShaderBuffer->GetBufferSize(), NULL,
        &m_brdfLUTPixelShader)))
    {
        return false;
    }


    D3D11_INPUT_ELEMENT_DESC skyboxPolygonLayout[1];
    skyboxPolygonLayout[0].SemanticName = "POSITION";
    skyboxPolygonLayout[0].SemanticIndex = 0;
    skyboxPolygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    skyboxPolygonLayout[0].InputSlot = 0;
    skyboxPolygonLayout[0].AlignedByteOffset = 0;
    skyboxPolygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    skyboxPolygonLayout[0].InstanceDataStepRate = 0;


    // 레이아웃의 요소 수를 가져옵니다.
    unsigned int numElements = sizeof(skyboxPolygonLayout) / sizeof(skyboxPolygonLayout[0]);

    // 정점 입력 레이아웃을 만듭니다.
    if (FAILED(device->CreateInputLayout(skyboxPolygonLayout, numElements, skyboxVertexShaderBuffer->GetBufferPointer(),
        skyboxVertexShaderBuffer->GetBufferSize(), &m_skyboxVertexLayout)))
    {
        return false;
    }

    // 정점 입력 레이아웃을 만듭니다.
    if (FAILED(device->CreateInputLayout(skyboxPolygonLayout, numElements, fullscreenVertexShaderBuffer->GetBufferPointer(),
        fullscreenVertexShaderBuffer->GetBufferSize(), &m_fullscreenVertexLayout)))
    {
        return false;
    }

    skyboxVertexShaderBuffer->Release();
    skyboxVertexShaderBuffer = 0;

    skyboxPixelShaderBuffer->Release();
    skyboxPixelShaderBuffer = 0;

    envMapPixelShaderBuffer->Release();
    envMapPixelShaderBuffer = 0;

    preFilterPixelShaderBuffer->Release();
    preFilterPixelShaderBuffer = 0;

    fullscreenVertexShaderBuffer->Release();
    fullscreenVertexShaderBuffer = 0;

    brdfLUTPixelShaderBuffer->Release();
    brdfLUTPixelShaderBuffer = 0;


    // 정점 셰이더에 있는 행렬 상수 버퍼의 구조체를 작성합니다.
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 합니다.
    if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer)))
    {
        return false;
    }

    D3D11_BUFFER_DESC specularBufferDesc;
    specularBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    specularBufferDesc.ByteWidth = sizeof(SpecularBufferType);
    specularBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    specularBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    specularBufferDesc.MiscFlags = 0;
    specularBufferDesc.StructureByteStride = 0;

    // 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 합니다.
    if (FAILED(device->CreateBuffer(&specularBufferDesc, NULL, &m_specularBuffer)))
    {
        return false;
    }


    return true;
}





void SkyboxShaderClass::ShutdownShader()
{
    // 테셀레이션 상수 버퍼를 해제합니다.
    if (m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    if (m_fullscreenVertexLayout)
    {
        m_fullscreenVertexLayout->Release();
        m_fullscreenVertexLayout = 0;
    }

    if (m_skyboxVertexLayout)
    {
        m_skyboxVertexLayout->Release();
        m_skyboxVertexLayout = 0;
    }

    if (m_brdfLUTPixelShader)
    {
        m_brdfLUTPixelShader->Release();
        m_brdfLUTPixelShader = 0;
    }

    if (m_envPreFilterPixelShader)
    {
        m_envPreFilterPixelShader->Release();
        m_envPreFilterPixelShader = 0;
    }

    if (m_envMapPixelShader)
    {
        m_envMapPixelShader->Release();
        m_envMapPixelShader = 0;
    }

    if (m_skyboxPixelShader)
    {
        m_skyboxPixelShader->Release();
        m_skyboxPixelShader = 0;
    }

    if (m_fullscreenVertexShader)
    {
        m_fullscreenVertexShader->Release();
        m_fullscreenVertexShader = 0;
    }

    if (m_skyboxVertexShader)
    {
        m_skyboxVertexShader->Release();
        m_skyboxVertexShader = 0;
    }
}




void SkyboxShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    // 에러 메시지를 출력창에 표시합니다.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // 에러 메세지를 반환합니다.
    errorMessage->Release();
    errorMessage = 0;

    // 컴파일 에러가 있음을 팝업 메세지로 알려줍니다.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}



bool SkyboxShaderClass::SetSkyboxShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* skyboxTexture)
{
    // 행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    // 상수 버퍼에 행렬을 복사합니다.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // 상수 버퍼의 잠금을 풉니다.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
    unsigned bufferNumber = 0;

    // 마지막으로 업데이트 된 값으로 버텍스 셰이더의 행렬 상수 버퍼를 설정합니다.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    deviceContext->PSSetShaderResources(0, 1, &skyboxTexture);

    return true;
}

bool SkyboxShaderClass::SetEnvMapShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* skyboxTexture)
{
    // 행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    // 상수 버퍼에 행렬을 복사합니다.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // 상수 버퍼의 잠금을 풉니다.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
    unsigned bufferNumber = 0;

    // 마지막으로 업데이트 된 값으로 버텍스 셰이더의 행렬 상수 버퍼를 설정합니다.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    deviceContext->PSSetShaderResources(0, 1, &skyboxTexture);

    return true;
}

bool SkyboxShaderClass::SetEnvPreFilterShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* skyboxTexture, float roughness)
{
    // 행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    // 상수 버퍼에 행렬을 복사합니다.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // 상수 버퍼의 잠금을 풉니다.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
    unsigned bufferNumber = 0;

    // 마지막으로 업데이트 된 값으로 버텍스 셰이더의 행렬 상수 버퍼를 설정합니다.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);




    if (FAILED(deviceContext->Map(m_specularBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    SpecularBufferType* dataPtr2 = (SpecularBufferType*)mappedResource.pData;

    dataPtr2->roughness = roughness;

    deviceContext->Unmap(m_specularBuffer, 0);

    bufferNumber = 0;

    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_specularBuffer);


    deviceContext->PSSetShaderResources(0, 1, &skyboxTexture);

    return true;
}

void SkyboxShaderClass::RenderSkyboxShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    deviceContext->IASetInputLayout(m_skyboxVertexLayout);

    // 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
    deviceContext->VSSetShader(m_skyboxVertexShader, NULL, 0);
    deviceContext->PSSetShader(m_skyboxPixelShader, NULL, 0);

    // 삼각형을 그립니다.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}

void SkyboxShaderClass::RenderEnvMapShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    deviceContext->IASetInputLayout(m_skyboxVertexLayout);

    // 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
    deviceContext->VSSetShader(m_skyboxVertexShader, NULL, 0);
    deviceContext->PSSetShader(m_envMapPixelShader, NULL, 0);

    // 삼각형을 그립니다.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}

void SkyboxShaderClass::RenderEnvPreFilterShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    deviceContext->IASetInputLayout(m_skyboxVertexLayout);

    // 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
    deviceContext->VSSetShader(m_skyboxVertexShader, NULL, 0);
    deviceContext->PSSetShader(m_envPreFilterPixelShader, NULL, 0);

    // 삼각형을 그립니다.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}

void SkyboxShaderClass::RenderBrdfLUTShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->IASetInputLayout(m_fullscreenVertexLayout);

    // 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
    deviceContext->VSSetShader(m_fullscreenVertexShader, NULL, 0);
    deviceContext->PSSetShader(m_brdfLUTPixelShader, NULL, 0);

    // 삼각형을 그립니다.
    deviceContext->Draw(4, 0);
}