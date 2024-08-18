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
    return InitializeShader(device, hwnd, L"FullscreenVS.HLSL", L"DeferredPS.HLSL");
}


void DeferredShaderClass::Shutdown()
{
    // ���ؽ� �� �ȼ� ���̴��� ���õ� ��ü�� �����մϴ�.
    ShutdownShader();
}


bool DeferredShaderClass::Render(ID3D11DeviceContext* deviceContext,
    XMMATRIX inverseProjection, XMMATRIX inverseView, int useAO, int useEnvMap,
    ID3D11ShaderResourceView* DepthTexture, 
    ID3D11ShaderResourceView* Diffuse, ID3D11ShaderResourceView* MetalRough,
    ID3D11ShaderResourceView* Normals, ID3D11ShaderResourceView* AO)
{
    // �������� ����� ���̴� �Ű� ������ �����մϴ�.
    if (!SetShaderParameters(deviceContext,
        inverseProjection, inverseView, useAO, useEnvMap,
        DepthTexture, Diffuse, MetalRough,
        Normals, AO))
    {
        return false;
    }

    // ������ ���۸� ���̴��� �������Ѵ�.
    RenderShader(deviceContext);

    return true;
}


bool DeferredShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    // ���ؽ� ���̴� �ڵ带 �������Ѵ�.
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "FullscreenVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
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
    D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

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
    XMMATRIX inverseProjection, XMMATRIX inverseView, int useAO, int useEnvMap,
    ID3D11ShaderResourceView* DepthTexture, ID3D11ShaderResourceView* Diffuse, ID3D11ShaderResourceView* MetalRough,
    ID3D11ShaderResourceView* Normals, ID3D11ShaderResourceView* AO)
{
    // �� ��� inverse �� ���ؾ� ���� �۵��ұ�
    // inverseProjection = XMMatrixTranspose(inverseProjection);
    // inverseView = XMMatrixTranspose(inverseView);


    // ��� ������ ������ �� �� �ֵ��� ��޴ϴ�.
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    if (FAILED(deviceContext->Map(m_deferredBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    DeferredCBufferType* dataPtr = (DeferredCBufferType*)mappedResource.pData;

    dataPtr->inverseProjection = inverseProjection;
    dataPtr->inverseView = inverseView;
    dataPtr->useAO = useAO;
    dataPtr->useEnvMap = useEnvMap;
    dataPtr->deferredCBufferpadding1 = 0;
    dataPtr->deferredCBufferpadding2 = 0;

    deviceContext->Unmap(m_deferredBuffer, 0);

    // ���� ���̴������� ��� ������ ��ġ�� �����մϴ�.
    unsigned int bufferNumber = 0;
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


void DeferredShaderClass::RenderShader(ID3D11DeviceContext* deviceContext)
{
    // ���� �Է� ���̾ƿ��� �����մϴ�.
    deviceContext->IASetInputLayout(m_layout);

    // �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // �ﰢ���� �׸��ϴ�.
    deviceContext->Draw(4, 0);
}
