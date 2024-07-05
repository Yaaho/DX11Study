#include "Stdafx.h"
#include "GBufferShaderClass.h"

GBufferShaderClass::GBufferShaderClass()
{
}


GBufferShaderClass::GBufferShaderClass(const GBufferShaderClass& other)
{
}


GBufferShaderClass::~GBufferShaderClass()
{
}


bool GBufferShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    // ���� �� �ȼ� ���̴��� �ʱ�ȭ�մϴ�.
    return InitializeShader(device, hwnd, L"GBufferVS.HLSL", L"GBufferPS.HLSL");
}


void GBufferShaderClass::Shutdown()
{
    // ���ؽ� �� �ȼ� ���̴��� ���õ� ��ü�� �����մϴ�.
    ShutdownShader();
}


bool GBufferShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
    XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
    ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* texture3, ID3D11ShaderResourceView* texture4, ID3D11ShaderResourceView* texture5,
    XMFLOAT4 gAlbedo, float gMetallic, float gRoughness, 
    int gUseAlbedoMap, int gUseOccMetalRough, int gUseAoMap, int gUseEmmisive, int gNormalState, int gConvertToLinear)
{
    // �������� ����� ���̴� �Ű� ������ �����մϴ�.
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, 
        texture1, texture2, texture3, texture4, texture5,
        gAlbedo, gMetallic, gRoughness,
        gUseAlbedoMap, gUseOccMetalRough, gUseAoMap, gUseEmmisive, gNormalState, gConvertToLinear))
    {
        return false;
    }

    // ������ ���۸� ���̴��� �������Ѵ�.
    RenderShader(deviceContext, indexCount);

    return true;
}


bool GBufferShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    // ���ؽ� ���̴� �ڵ带 �������Ѵ�.
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "GBufferVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
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
    result = D3DCompileFromFile(psFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GBufferPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
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
    D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
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

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    polygonLayout[3].SemanticName = "TANGENT";
    polygonLayout[3].SemanticIndex = 0;
    polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[3].InputSlot = 0;
    polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[3].InstanceDataStepRate = 0;

    polygonLayout[4].SemanticName = "BINORMAL";
    polygonLayout[4].SemanticIndex = 0;
    polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[4].InputSlot = 0;
    polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[4].InstanceDataStepRate = 0;


    // ���̾ƿ��� ��� ���� �����ɴϴ�.
    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

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

    // ���ؽ� ���̴����ִ� ���� ��� ��� ������ ����ü�� �����մϴ�.
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

    D3D11_BUFFER_DESC PBRMaterialBufferDesc;
    PBRMaterialBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    PBRMaterialBufferDesc.ByteWidth = sizeof(PBRMaterialBufferType);
    PBRMaterialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    PBRMaterialBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    PBRMaterialBufferDesc.MiscFlags = 0;
    PBRMaterialBufferDesc.StructureByteStride = 0;

    // �� Ŭ���� ������ �ȼ� ���̴� ��� ���ۿ� ������ �� �� �ַ� ��� ���� �����͸� �����.
    if (FAILED(device->CreateBuffer(&PBRMaterialBufferDesc, NULL, &m_PBRMaterialBuffer)))
    {
        return false;
    }



    return true;
}


void GBufferShaderClass::ShutdownShader()
{
    if (m_PBRMaterialBuffer)
    {
        m_PBRMaterialBuffer->Release();
        m_PBRMaterialBuffer = 0;
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


void GBufferShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    // ���� �޽����� ���â�� ǥ���մϴ�.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // ���� �޼����� ��ȯ�մϴ�.
    errorMessage->Release();
    errorMessage = 0;

    // ������ ������ ������ �˾� �޼����� �˷��ݴϴ�.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool GBufferShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
    ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* texture3, ID3D11ShaderResourceView* texture4, ID3D11ShaderResourceView* texture5,
    XMFLOAT4 gAlbedo, float gMetallic, float gRoughness,
    int gUseAlbedoMap, int gUseOccMetalRough, int gUseAoMap, int gUseEmmisive, int gNormalState, int gConvertToLinear)
{
    // ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �մϴ�
    // GPU�� ���켱�̱� ������, CPU���� ������ ���� ���켱���� ���� �����ָ� �ȴ�.
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


    // light constant buffer �� ����� �� �ֵ��� ��ٴ�
    if (FAILED(deviceContext->Map(m_PBRMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // constant ������ �����Ϳ� ���� �����͸� �����´�.
    PBRMaterialBufferType* dataptr2 = (PBRMaterialBufferType*)mappedResource.pData;

    // ���� ������ constant ���ۿ� �����Ѵ�.
    dataptr2->gAlbedo = gAlbedo;
    dataptr2->gMetallic = gMetallic;
    dataptr2->gRoughness = gRoughness;
    dataptr2->gUseAlbedoMap = gUseAlbedoMap;
    dataptr2->gUseOccMetalRough = gUseOccMetalRough;
    dataptr2->gUseAoMap = gUseAoMap;
    dataptr2->gUseEmmisive = gUseEmmisive;
    dataptr2->gNormalState = gNormalState;
    dataptr2->gConvertToLinear = gConvertToLinear;


    // constant ������ ����� �����Ѵ�.
    deviceContext->Unmap(m_PBRMaterialBuffer, 0);

    // �ȼ� ���̴����� ���� constant ������ ��ġ�� �����Ѵ�.
    bufferNumber = 0;

    // ���������� ������Ʈ �� ������ �ȼ� ���̴����� ���� ��� ���۸� �����Ѵ�.
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_PBRMaterialBuffer);


    deviceContext->PSSetShaderResources(0, 1, &texture1);
    deviceContext->PSSetShaderResources(1, 1, &texture2);
    deviceContext->PSSetShaderResources(2, 1, &texture3);
    deviceContext->PSSetShaderResources(3, 1, &texture4);
    deviceContext->PSSetShaderResources(4, 1, &texture5);

    return true;
}


void GBufferShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // ���� �Է� ���̾ƿ��� �����մϴ�.
    deviceContext->IASetInputLayout(m_layout);

    // �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����մϴ�.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // �ﰢ���� �׸��ϴ�.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}