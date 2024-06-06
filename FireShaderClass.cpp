#include "stdafx.h"
#include "FireShaderClass.h"


FireShaderClass::FireShaderClass()
{
}


FireShaderClass::FireShaderClass(const FireShaderClass& other)
{
}


FireShaderClass::~FireShaderClass()
{
}


bool FireShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    // ���� �� �ȼ� ���̴��� �ʱ�ȭ�մϴ�.
    return InitializeShader(device, hwnd, L"FireVS.HLSL", L"FirePS.HLSL");
}


void FireShaderClass::Shutdown()
{
    // ���ؽ� �� �ȼ� ���̴��� ���õ� ��ü�� �����մϴ�.
    ShutdownShader();
}


bool FireShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
    XMMATRIX projectionMatrix, ID3D11ShaderResourceView* fireTexture,
    ID3D11ShaderResourceView* noiseTexture, ID3D11ShaderResourceView* alphaTexture,
    float frameTime, XMFLOAT3 scrollSpeeds, XMFLOAT3 scales, XMFLOAT2 distortion1,
    XMFLOAT2 distortion2, XMFLOAT2 distortion3, float distortionScale, float distortionBias)
{
    // �������� ����� ���̴� �Ű� ������ �����մϴ�.
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, fireTexture, noiseTexture,
        alphaTexture, frameTime, scrollSpeeds, scales, distortion1, distortion2, distortion3,
        distortionScale, distortionBias))
    {
        return false;
    }

    // ������ ���۸� ���̴��� �������Ѵ�.
    RenderShader(deviceContext, indexCount);

    return true;
}


bool FireShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    // ���ؽ� ���̴� �ڵ带 �������Ѵ�.
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "FireVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
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
    result = D3DCompileFromFile(psFilename, NULL, NULL, "FirePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
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

    // ���ؽ� ���̴����ִ� ���� ��� ��� ������ ������ �����մϴ�.
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

    // ���ؽ� ���̴����ִ� ���� ������ ��� ������ ������ �����մϴ�.
    D3D11_BUFFER_DESC noiseBufferDesc;
    noiseBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    noiseBufferDesc.ByteWidth = sizeof(NoiseBufferType);
    noiseBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    noiseBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    noiseBufferDesc.MiscFlags = 0;
    noiseBufferDesc.StructureByteStride = 0;

    // �� Ŭ���� ������ ���� ���̴� ��� ���ۿ� �׼��� �� �� �ֵ��� ������ ���� �����͸� ����ϴ�.
    result = device->CreateBuffer(&noiseBufferDesc, NULL, &m_noiseBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // �ȼ� ���̴����ִ� ���� �ְ� ��� ������ ������ �����մϴ�.
    D3D11_BUFFER_DESC distortionBufferDesc;
    distortionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    distortionBufferDesc.ByteWidth = sizeof(DistortionBufferType);
    distortionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    distortionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    distortionBufferDesc.MiscFlags = 0;
    distortionBufferDesc.StructureByteStride = 0;

    // �� Ŭ���� ������ �ȼ� ���̴� ��� ���ۿ� �׼��� �� �� �ֵ��� �ְ� ���� �����͸� ����ϴ�.
    result = device->CreateBuffer(&distortionBufferDesc, NULL, &m_distortionBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // �ؽ�ó ���÷� ���� ������ ����ϴ�.
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

    // Ŭ���� ���÷��� ���� �� ��° �ؽ�ó ���÷� ���� ������ ����ϴ�.
    D3D11_SAMPLER_DESC samplerDesc2;
    samplerDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc2.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc2.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc2.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc2.MipLODBias = 0.0f;
    samplerDesc2.MaxAnisotropy = 1;
    samplerDesc2.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc2.BorderColor[0] = 0;
    samplerDesc2.BorderColor[1] = 0;
    samplerDesc2.BorderColor[2] = 0;
    samplerDesc2.BorderColor[3] = 0;
    samplerDesc2.MinLOD = 0;
    samplerDesc2.MaxLOD = D3D11_FLOAT32_MAX;

    // �ؽ�ó ���÷� ���¸� ����ϴ�.
    result = device->CreateSamplerState(&samplerDesc2, &m_sampleState2);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}


void FireShaderClass::ShutdownShader()
{
    // �� ��° ���÷� ���¸� �����մϴ�.
    if (m_sampleState2)
    {
        m_sampleState2->Release();
        m_sampleState2 = 0;
    }

    // ���÷� ���¸� �����Ѵ�.
    if (m_sampleState)
    {
        m_sampleState->Release();
        m_sampleState = 0;
    }

    // �ְ� ��� ���۸� �����մϴ�.
    if (m_distortionBuffer)
    {
        m_distortionBuffer->Release();
        m_distortionBuffer = 0;
    }

    // ���� ��� ���۸� �����Ѵ�.
    if (m_noiseBuffer)
    {
        m_noiseBuffer->Release();
        m_noiseBuffer = 0;
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


void FireShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    // ���� �޽����� ���â�� ǥ���մϴ�.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // ���� �޼����� ��ȯ�մϴ�.
    errorMessage->Release();
    errorMessage = 0;

    // ������ ������ ������ �˾� �޼����� �˷��ݴϴ�.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}


bool FireShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
    XMMATRIX projectionMatrix, ID3D11ShaderResourceView* fireTexture,
    ID3D11ShaderResourceView* noiseTexture, ID3D11ShaderResourceView* alphaTexture,
    float frameTime, XMFLOAT3 scrollSpeeds, XMFLOAT3 scales, XMFLOAT2 distortion1,
    XMFLOAT2 distortion2, XMFLOAT2 distortion3, float distortionScale,
    float distortionBias)
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

    // �� �� �ֵ��� ������ ��� ���۸� ��޴ϴ�.
    if (FAILED(deviceContext->Map(m_noiseBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // ������ ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
    NoiseBufferType* dataPtr2 = (NoiseBufferType*)mappedResource.pData;

    // �����͸� ������ ��� ���ۿ� �����մϴ�.
    dataPtr2->frameTime = frameTime;
    dataPtr2->scrollSpeeds = scrollSpeeds;
    dataPtr2->scales = scales;
    dataPtr2->padding = 0.0f;

    // ������ ��� ������ ����� �����մϴ�.
    deviceContext->Unmap(m_noiseBuffer, 0);

    // ���ؽ� ���̴����� ������ ��� ������ ��ġ�� ??�����մϴ�.
    bufferNumber = 1;

    // ���� ���ؽ� ���̴����� ������Ʈ �� ������ ������ ��� ���۸� �����մϴ�.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_noiseBuffer);

    // �� �� �ֵ��� �ְ� ��� ���۸� ��޴ϴ�.
    if (FAILED(deviceContext->Map(m_distortionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // �ְ� ��� ������ �����Ϳ� ���� �����͸� �����ɴϴ�.
    DistortionBufferType* dataPtr3 = (DistortionBufferType*)mappedResource.pData;

    // �ְ� ��� ���ۿ� �����͸� �����մϴ�.
    dataPtr3->distortion1 = distortion1;
    dataPtr3->distortion2 = distortion2;
    dataPtr3->distortion3 = distortion3;
    dataPtr3->distortionScale = distortionScale;
    dataPtr3->distortionBias = distortionBias;

    // �ְ� ��� ������ ����� �����մϴ�.
    deviceContext->Unmap(m_distortionBuffer, 0);

    // �ȼ� ���̴����� �ְ� ��� ������ ��ġ�� ??�����մϴ�.
    bufferNumber = 0;

    // ���� �ȼ� ���̴����� �ְ� ��� ���۸� ������Ʈ �� ������ �����մϴ�.
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_distortionBuffer);

    // �ȼ� ���̴��� 3 ���� ���̴� �ؽ�ó ���ҽ��� �����մϴ�.
    deviceContext->PSSetShaderResources(0, 1, &fireTexture);
    deviceContext->PSSetShaderResources(1, 1, &noiseTexture);
    deviceContext->PSSetShaderResources(2, 1, &alphaTexture);

    return true;
}


void FireShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // ���� �Է� ���̾ƿ��� �����մϴ�.
    deviceContext->IASetInputLayout(m_layout);

    // �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // �ȼ� ���̴����� ���÷� ���¸� �����մϴ�.
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);
    deviceContext->PSSetSamplers(1, 1, &m_sampleState2);

    // �ﰢ���� �������մϴ�.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
