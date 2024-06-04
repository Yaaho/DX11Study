#include "Stdafx.h"
#include "GlassShaderClass.h"


GlassShaderClass::GlassShaderClass()
{
}

GlassShaderClass::GlassShaderClass(const GlassShaderClass& other)
{
}

GlassShaderClass::~GlassShaderClass()
{
}

bool GlassShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    // ���� �� �ȼ� ���̴��� �ʱ�ȭ�մϴ�.
    return InitializeShader(device, hwnd, L"GlassVS.HLSL", L"GlassPS.HLSL");
}

void GlassShaderClass::Shutdown()
{
    // ���ؽ� �� �ȼ� ���̴��� ���õ� ��ü�� �����մϴ�.
    ShutdownShader();
}

bool GlassShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
    XMMATRIX projectionMatrix, ID3D11ShaderResourceView* colorTexture,
    ID3D11ShaderResourceView* normalTexture, ID3D11ShaderResourceView* refractionTexture,
    float refractionScale)
{
    // �������� ����� ���̴� �Ű� ������ �����մϴ�.
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, colorTexture,
        normalTexture, refractionTexture, refractionScale))
    {
        return false;
    }

    // ������ ���۸� ���̴��� �������Ѵ�.
    RenderShader(deviceContext, indexCount);

    return true;
}

bool GlassShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    // ���ؽ� ���̴� �ڵ带 �������Ѵ�.
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "GlassVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &vertexShaderBuffer, &errorMessage);
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
    result = D3DCompileFromFile(psFilename, NULL, NULL, "GlassPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &pixelShaderBuffer, &errorMessage);
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

    // ���ؽ� ���̴����ִ� ��� ���� ��� ������ ������ �����մϴ�.
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // �� Ŭ���� ������ ���� ���̴� ��� ���ۿ� �׼��� �� �� �ֵ��� ��� ���� �����͸� ����ϴ�.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // �ȼ� ���̴����ִ� ���� ���� ��� ������ ������ �����մϴ�.
    D3D11_BUFFER_DESC glassBufferDesc;
    glassBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    glassBufferDesc.ByteWidth = sizeof(GlassBufferType);
    glassBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    glassBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    glassBufferDesc.MiscFlags = 0;
    glassBufferDesc.StructureByteStride = 0;

    // �� Ŭ���� ������ �ȼ� ���̴� ��� ���ۿ� �׼��� �� �� �ֵ��� ��� ���� �����͸� ����ϴ�.
    result = device->CreateBuffer(&glassBufferDesc, NULL, &m_glassBuffer);
    if (FAILED(result))
    {
        return false;
    }

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

    return true;
}


void GlassShaderClass::ShutdownShader()
{
    // ���÷� ���¸� �����Ѵ�.
    if (m_sampleState)
    {
        m_sampleState->Release();
        m_sampleState = 0;
    }

    // ���� ��� ���۸� �����ϴ�.
    if (m_glassBuffer)
    {
        m_glassBuffer->Release();
        m_glassBuffer = 0;
    }

    // ��� ��� ���۸� �����մϴ�.
    if (m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    // ���̾ƿ��� �����մϴ�.
    if (m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    // �ȼ� ���̴��� �����ϴ�.
    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    // ���ؽ� ���̴��� �����ϴ�.
    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }
}



void GlassShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    // ���� �޽����� ���â�� ǥ���մϴ�.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // ���� �޼����� ��ȯ�մϴ�.
    errorMessage->Release();
    errorMessage = 0;

    // ������ ������ ������ �˾� �޼����� �˷��ݴϴ�.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}


bool GlassShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
    XMMATRIX projectionMatrix, ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture,
    ID3D11ShaderResourceView* refractionTexture, float refractionScale)
{
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    deviceContext->Unmap(m_matrixBuffer, 0);

    unsigned int bufferNumber = 0;

    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    if (FAILED(deviceContext->Map(m_glassBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    GlassBufferType* dataPtr2 = (GlassBufferType*)mappedResource.pData;

    dataPtr2->refractionScale = refractionScale;
    dataPtr2->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

    deviceContext->Unmap(m_glassBuffer, 0);

    bufferNumber = 0;

    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_glassBuffer);

    deviceContext->PSSetShaderResources(0, 1, &colorTexture);
    deviceContext->PSSetShaderResources(1, 1, &normalTexture);
    deviceContext->PSSetShaderResources(2, 1, &refractionTexture);

    return true;
}

void GlassShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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