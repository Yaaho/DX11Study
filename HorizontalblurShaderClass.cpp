#include "Stdafx.h"
#include "HorizontalblurShaderClass.h"


HorizontalblurShaderClass::HorizontalblurShaderClass()
{
}


HorizontalblurShaderClass::HorizontalblurShaderClass(const HorizontalblurShaderClass& other)
{
}


HorizontalblurShaderClass::~HorizontalblurShaderClass()
{
}


bool HorizontalblurShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    // ���� �� �ȼ� ���̴��� �ʱ�ȭ�մϴ�.
    return InitializeShader(device, hwnd, L"HorizontalblurVS.HLSL", L"HorizontalblurPS.HLSL");
}


void HorizontalblurShaderClass::Shutdown()
{
    // ���ؽ� �� �ȼ� ���̴��� ���õ� ��ü�� �����մϴ�.
    ShutdownShader();
}


bool HorizontalblurShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix,
    XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float screenWidth)
{
    // �������� ����� ���̴� �Ű� ������ �����մϴ�.
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, screenWidth))
    {
        return false;
    }

    // ������ ���۸� ���̴��� �������Ѵ�.
    RenderShader(deviceContext, indexCount);

    return true;
}


bool HorizontalblurShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename,
    const WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    // ���ؽ� ���̴� �ڵ带 �������Ѵ�.
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "HorizontalblurVertexShader", "vs_5_0",
        D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // ���̴� ������ ���н� �����޽����� ����մϴ�.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        // ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����Դϴ�.
        else
        {
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // �ȼ� ���̴� �ڵ带 �������Ѵ�.
    ID3D10Blob* pixelShaderBuffer = nullptr;
    result = D3DCompileFromFile(psFilename, NULL, NULL, "HorizontalblurPixelShader", "ps_5_0",
        D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // ���̴� ������ ���н� �����޽����� ����մϴ�.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        // ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����Դϴ�.
        else
        {
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // ���۷κ��� ���� ���̴��� �����Ѵ�.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL,
        &m_vertexShader);
    if (FAILED(result))
    {
        return false;
    }

    // ���ۿ��� �ȼ� ���̴��� �����մϴ�.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL,
        &m_pixelShader);
    if (FAILED(result))
    {
        return false;
    }

    // ���� �Է� ���̾ƿ� ����ü�� �����մϴ�.
    // �� ������ ModelClass�� ���̴��� VertexType ������ ��ġ�ؾ��մϴ�.
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    // ���̾ƿ��� ��� ���� �����ɴϴ�.
    UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // ���� �Է� ���̾ƿ��� ����ϴ�.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_layout);
    if (FAILED(result))
    {
        return false;
    }

    // �� �̻� ������ �ʴ� ���� ���̴� �۹��� �ȼ� ���̴� ���۸� �����մϴ�.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    // �ؽ�ó ���÷� ���� ����ü�� ���� �� �����մϴ�.
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // �ؽ�ó ���÷� ���¸� ����ϴ�.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
    if (FAILED(result))
    {
        return false;
    }

    // ���� ���̴��� �ִ� ��� ��� ������ ����ü�� �ۼ��մϴ�.
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �մϴ�.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // �ȼ� ���̴����ִ� ���� ���� ��� ������ ������ �����մϴ�.
    // D3D11_BIND_CONSTANT_BUFFER�� ����ϸ� ByteWidth�� �׻� 16�� ��� �����ϸ� �׷��� ������ CreateBuffer�� �����մϴ�.
    D3D11_BUFFER_DESC screenSizeBufferDesc;
    screenSizeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    screenSizeBufferDesc.ByteWidth = sizeof(ScreenSizeBufferType);
    screenSizeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    screenSizeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    screenSizeBufferDesc.MiscFlags = 0;
    screenSizeBufferDesc.StructureByteStride = 0;

    // �� Ŭ���� ������ ���� ���̴� ��� ���ۿ� �׼��� �� �� �ֵ��� ��� ���� �����͸� ����ϴ�.
    result = device->CreateBuffer(&screenSizeBufferDesc, NULL, &m_screenSizeBuffer);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}


void HorizontalblurShaderClass::ShutdownShader()
{
    // ȭ�� ũ�� ��� ���۸� �����մϴ�.
    if (m_screenSizeBuffer)
    {
        m_screenSizeBuffer->Release();
        m_screenSizeBuffer = 0;
    }

    // ��� ��� ���۸� �����մϴ�.
    if (m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    // ���÷� ���¸� �����Ѵ�.
    if (m_sampleState)
    {
        m_sampleState->Release();
        m_sampleState = 0;
    }

    // ���̾ƿ��� �����մϴ�.
    if (m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    // �ȼ� ���̴��� �����մϴ�.
    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    // ���ؽ� ���̴��� �����մϴ�.
    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }
}


void HorizontalblurShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    // ���� �޽����� ���â�� ǥ���մϴ�.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // ���� �޼����� ��ȯ�մϴ�.
    errorMessage->Release();
    errorMessage = 0;

    // ������ ������ ������ �˾� �޼����� �˷��ݴϴ�.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool HorizontalblurShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
    XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float screenWidth)
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
    unsigned int bufferNumber = 0;

    // ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲߴϴ�.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    // m_screenSizeBuffer�� ��� �� �ֵ��� ����Ѵ�.
    if (FAILED(deviceContext->Map(m_screenSizeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
    ScreenSizeBufferType* dataPtr2 = (ScreenSizeBufferType*)mappedResource.pData;

    // ���� ������ ��� ���ۿ� �����մϴ�.
    dataPtr2->screenWidth = screenWidth;
    dataPtr2->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);;

    // ��� ������ ����� �����մϴ�.
    deviceContext->Unmap(m_screenSizeBuffer, 0);

    // �ȼ� ���̴����� ���� ��� ������ ��ġ�� ??�����մϴ�.
    bufferNumber = 1;

    // ���� ������Ʈ �� ������ ���ؽ� ���̴����� ��� ���۸� �����մϴ�.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_screenSizeBuffer);

    // �ȼ� ���̴����� ���̴� �ؽ�ó ���ҽ��� �����մϴ�.
    deviceContext->PSSetShaderResources(0, 1, &texture);

    return true;
}


void HorizontalblurShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // ���� �Է� ���̾ƿ��� �����մϴ�.
    deviceContext->IASetInputLayout(m_layout);

    // �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // �ȼ� ���̴����� ���÷� ���¸� �����մϴ�.
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);

    // �ﰢ���� �׸��ϴ�.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}