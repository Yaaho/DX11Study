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
    // ���ؽ� �� �ȼ� ���̴��� ���õ� ��ü�� �����մϴ�.
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
        // ���̴� ������ ���н� �����޽����� ����մϴ�.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsSkyboxFilename);
        }
        // ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����Դϴ�.
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
        // ���̴� ������ ���н� �����޽����� ����մϴ�.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psSkyboxFilename);
        }
        // ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����Դϴ�.
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
        // ���̴� ������ ���н� �����޽����� ����մϴ�.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psEnvMapFilename);
        }
        // ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����Դϴ�.
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
        // ���̴� ������ ���н� �����޽����� ����մϴ�.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psPreFilterFilename);
        }
        // ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����Դϴ�.
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
        // ���̴� ������ ���н� �����޽����� ����մϴ�.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFullscreenFilename);
        }
        // ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����Դϴ�.
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
        // ���̴� ������ ���н� �����޽����� ����մϴ�.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psBrdfLUTFilename);
        }
        // ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����Դϴ�.
        else
        {
            MessageBox(hwnd, psBrdfLUTFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }



    // ���۷κ��� ���� ���̴��� �����Ѵ�.
    if (FAILED(device->CreateVertexShader(skyboxVertexShaderBuffer->GetBufferPointer(), skyboxVertexShaderBuffer->GetBufferSize(), NULL,
        &m_skyboxVertexShader)))
    {
        return false;
    }

    // ���۷κ��� �ȼ� ���̴��� �����Ѵ�.
    if (FAILED(device->CreatePixelShader(skyboxPixelShaderBuffer->GetBufferPointer(), skyboxPixelShaderBuffer->GetBufferSize(), NULL,
        &m_skyboxPixelShader)))
    {
        return false;
    }

    // ���۷κ��� �ȼ� ���̴��� �����Ѵ�.
    if (FAILED(device->CreatePixelShader(envMapPixelShaderBuffer->GetBufferPointer(), envMapPixelShaderBuffer->GetBufferSize(), NULL,
        &m_envMapPixelShader)))
    {
        return false;
    }

    // ���۷κ��� �ȼ� ���̴��� �����Ѵ�.
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


    // ���̾ƿ��� ��� ���� �����ɴϴ�.
    unsigned int numElements = sizeof(skyboxPolygonLayout) / sizeof(skyboxPolygonLayout[0]);

    // ���� �Է� ���̾ƿ��� ����ϴ�.
    if (FAILED(device->CreateInputLayout(skyboxPolygonLayout, numElements, skyboxVertexShaderBuffer->GetBufferPointer(),
        skyboxVertexShaderBuffer->GetBufferSize(), &m_skyboxVertexLayout)))
    {
        return false;
    }

    // ���� �Է� ���̾ƿ��� ����ϴ�.
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


    // ���� ���̴��� �ִ� ��� ��� ������ ����ü�� �ۼ��մϴ�.
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �մϴ�.
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

    // ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �մϴ�.
    if (FAILED(device->CreateBuffer(&specularBufferDesc, NULL, &m_specularBuffer)))
    {
        return false;
    }


    return true;
}





void SkyboxShaderClass::ShutdownShader()
{
    // �׼����̼� ��� ���۸� �����մϴ�.
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
    // ���� �޽����� ���â�� ǥ���մϴ�.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // ���� �޼����� ��ȯ�մϴ�.
    errorMessage->Release();
    errorMessage = 0;

    // ������ ������ ������ �˾� �޼����� �˷��ݴϴ�.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}



bool SkyboxShaderClass::SetSkyboxShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* skyboxTexture)
{
    // ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �մϴ�
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    // ��� ���ۿ� ����� �����մϴ�.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // ��� ������ ����� Ǳ�ϴ�.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
    unsigned bufferNumber = 0;

    // ���������� ������Ʈ �� ������ ���ؽ� ���̴��� ��� ��� ���۸� �����մϴ�.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    deviceContext->PSSetShaderResources(0, 1, &skyboxTexture);

    return true;
}

bool SkyboxShaderClass::SetEnvMapShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* skyboxTexture)
{
    // ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �մϴ�
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    // ��� ���ۿ� ����� �����մϴ�.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // ��� ������ ����� Ǳ�ϴ�.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
    unsigned bufferNumber = 0;

    // ���������� ������Ʈ �� ������ ���ؽ� ���̴��� ��� ��� ���۸� �����մϴ�.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    deviceContext->PSSetShaderResources(0, 1, &skyboxTexture);

    return true;
}

bool SkyboxShaderClass::SetEnvPreFilterShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* skyboxTexture, float roughness)
{
    // ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �մϴ�
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    // ��� ���ۿ� ����� �����մϴ�.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // ��� ������ ����� Ǳ�ϴ�.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
    unsigned bufferNumber = 0;

    // ���������� ������Ʈ �� ������ ���ؽ� ���̴��� ��� ��� ���۸� �����մϴ�.
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

    // �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
    deviceContext->VSSetShader(m_skyboxVertexShader, NULL, 0);
    deviceContext->PSSetShader(m_skyboxPixelShader, NULL, 0);

    // �ﰢ���� �׸��ϴ�.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}

void SkyboxShaderClass::RenderEnvMapShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    deviceContext->IASetInputLayout(m_skyboxVertexLayout);

    // �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
    deviceContext->VSSetShader(m_skyboxVertexShader, NULL, 0);
    deviceContext->PSSetShader(m_envMapPixelShader, NULL, 0);

    // �ﰢ���� �׸��ϴ�.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}

void SkyboxShaderClass::RenderEnvPreFilterShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    deviceContext->IASetInputLayout(m_skyboxVertexLayout);

    // �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
    deviceContext->VSSetShader(m_skyboxVertexShader, NULL, 0);
    deviceContext->PSSetShader(m_envPreFilterPixelShader, NULL, 0);

    // �ﰢ���� �׸��ϴ�.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}

void SkyboxShaderClass::RenderBrdfLUTShader(ID3D11DeviceContext* deviceContext)
{
    deviceContext->IASetInputLayout(m_fullscreenVertexLayout);

    // �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
    deviceContext->VSSetShader(m_fullscreenVertexShader, NULL, 0);
    deviceContext->PSSetShader(m_brdfLUTPixelShader, NULL, 0);

    // �ﰢ���� �׸��ϴ�.
    deviceContext->Draw(4, 0);
}