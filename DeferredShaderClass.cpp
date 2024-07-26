#include "Stdafx.h"
#include "DeferredShaderClass.h"


DeferredShaderClass::DeferredShaderClass()
{
}


DeferredShaderClass::DeferredShaderClass(const DeferredShaderClass& other)
{
}


DeferredShaderClass::~DeferredShaderClass()
{
}


bool DeferredShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    // ���� �� �ȼ� ���̴��� �ʱ�ȭ�մϴ�.
    return InitializeShader(device, hwnd, L"DeferredVS.HLSL", L"DeferredPS.HLSL");
}


void DeferredShaderClass::Shutdown()
{
    // ���ؽ� �� �ȼ� ���̴��� ���õ� ��ü�� �����մϴ�.
    ShutdownShader();
}


bool DeferredShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
    XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
    XMMATRIX inverseProjection, XMMATRIX inverseView, int useAO, int useEnvMap,
    ID3D11ShaderResourceView* DepthTexture, 
    ID3D11ShaderResourceView* Diffuse, ID3D11ShaderResourceView* MetalRough,
    ID3D11ShaderResourceView* Normals, ID3D11ShaderResourceView* AO)
{
    // �������� ����� ���̴� �Ű� ������ �����մϴ�.
    if (!SetShaderParameters(deviceContext, 
        worldMatrix, viewMatrix, projectionMatrix,
        inverseProjection, inverseView, useAO, useEnvMap,
        DepthTexture, Diffuse, MetalRough,
        Normals, AO))
    {
        return false;
    }

    // ������ ���۸� ���̴��� �������Ѵ�.
    RenderShader(deviceContext, indexCount);

    return true;
}


bool DeferredShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    // ���ؽ� ���̴� �ڵ带 �������Ѵ�.
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "DeferredVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
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
    // ���̴� ���� ���� include �� ������ D3D_COMPILE_STANDARD_FILE_INCLUDE �� ����ؾ� �Ѵ�.
    result = D3DCompileFromFile(psFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "DeferredPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
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


    D3D11_BUFFER_DESC deferredBufferDesc;
    deferredBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    deferredBufferDesc.ByteWidth = sizeof(DeferredCBufferType);
    deferredBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    deferredBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    deferredBufferDesc.MiscFlags = 0;
    deferredBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&deferredBufferDesc, NULL, &m_deferredBuffer);
    if (FAILED(result))
    {
        return false;
    }


    return true;
}


void DeferredShaderClass::ShutdownShader()
{
    if (m_deferredBuffer)
    {
        m_deferredBuffer->Release();
        m_deferredBuffer = 0;
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


void DeferredShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    // ���� �޽����� ���â�� ǥ���մϴ�.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // ���� �޼����� ��ȯ�մϴ�.
    errorMessage->Release();
    errorMessage = 0;

    // ������ ������ ������ �˾� �޼����� �˷��ݴϴ�.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool DeferredShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
    XMMATRIX inverseProjection, XMMATRIX inverseView, int useAO, int useEnvMap,
    ID3D11ShaderResourceView* DepthTexture, ID3D11ShaderResourceView* Diffuse, ID3D11ShaderResourceView* MetalRough,
    ID3D11ShaderResourceView* Normals, ID3D11ShaderResourceView* AO)
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



    if (FAILED(deviceContext->Map(m_deferredBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    DeferredCBufferType* dataPtr2 = (DeferredCBufferType*)mappedResource.pData;

    dataPtr2->inverseProjection = inverseProjection;
    dataPtr2->inverseView = inverseView;
    dataPtr2->useAO = useAO;
    dataPtr2->useEnvMap = useEnvMap;

    deviceContext->Unmap(m_deferredBuffer, 0);

    bufferNumber = 3;

    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_deferredBuffer);


    deviceContext->PSSetShaderResources(0, 1, &DepthTexture);
    deviceContext->PSSetShaderResources(1, 1, &Diffuse);
    deviceContext->PSSetShaderResources(2, 1, &MetalRough);
    deviceContext->PSSetShaderResources(3, 1, &Normals);
    // 4 �� ������ ��
    deviceContext->PSSetShaderResources(5, 1, &AO);

    return true;
}


void DeferredShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // ���� �Է� ���̾ƿ��� �����մϴ�.
    deviceContext->IASetInputLayout(m_layout);

    // �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // �ﰢ���� �׸��ϴ�.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
