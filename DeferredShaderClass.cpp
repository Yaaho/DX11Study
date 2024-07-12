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
    // 정점 및 픽셀 쉐이더를 초기화합니다.
    return InitializeShader(device, hwnd, L"DeferredVS.HLSL", L"DeferredPS.HLSL");
}


void DeferredShaderClass::Shutdown()
{
    // 버텍스 및 픽셀 쉐이더와 관련된 객체를 종료합니다.
    ShutdownShader();
}


bool DeferredShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
    XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
    XMFLOAT4X4 inverseProjection, XMFLOAT4X4 inverseView, int useAO, int useEnvMap,
    ID3D11ShaderResourceView* DepthTexture, ID3D11ShaderResourceView* Diffuse, ID3D11ShaderResourceView* MetalRough,
    ID3D11ShaderResourceView* Normals, ID3D11ShaderResourceView* AO,
    ID3D11ShaderResourceView* EnvMap, ID3D11ShaderResourceView* PrefilteredSpecMap, ID3D11ShaderResourceView* BrdfLUT,
    ID3D11ShaderResourceView* Emissive)
{
    // 렌더링에 사용할 셰이더 매개 변수를 설정합니다.
    if (!SetShaderParameters(deviceContext, 
        worldMatrix, viewMatrix, projectionMatrix,
        inverseProjection, inverseView, useAO, useEnvMap,
        DepthTexture, Diffuse, MetalRough,
        Normals, AO,
        EnvMap, PrefilteredSpecMap, BrdfLUT,
        Emissive))
    {
        return false;
    }

    // 설정된 버퍼를 셰이더로 렌더링한다.
    RenderShader(deviceContext, indexCount);

    return true;
}


bool DeferredShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    // 버텍스 쉐이더 코드를 컴파일한다.
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "DeferredVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &vertexShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // 셰이더 컴파일 실패시 오류메시지를 출력합니다.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        // 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
        else
        {
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // 픽셀 쉐이더 코드를 컴파일한다.
    ID3D10Blob* pixelShaderBuffer = nullptr;
    // 세이더 파일 내에 include 가 있으면 D3D_COMPILE_STANDARD_FILE_INCLUDE 를 사용해야 한다.
    result = D3DCompileFromFile(psFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "DeferredPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
        &pixelShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // 셰이더 컴파일 실패시 오류메시지를 출력합니다.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        // 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
        else
        {
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // 버퍼로부터 정점 셰이더를 생성한다.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL,
        &m_vertexShader);
    if (FAILED(result))
    {
        return false;
    }

    // 버퍼에서 픽셀 쉐이더를 생성합니다.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL,
        &m_pixelShader);
    if (FAILED(result))
    {
        return false;
    }

    // 정점 입력 레이아웃 구조체를 설정합니다.
    // 이 설정은 ModelClass와 셰이더의 VertexType 구조와 일치해야합니다.
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

    // 레이아웃의 요소 수를 가져옵니다.
    UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // 정점 입력 레이아웃을 만듭니다.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_layout);
    if (FAILED(result))
    {
        return false;
    }

    // 더 이상 사용되지 않는 정점 셰이더 퍼버와 픽셀 셰이더 버퍼를 해제합니다.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    // 정점 셰이더에 있는 행렬 상수 버퍼의 구조체를 작성합니다.
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 합니다.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if (FAILED(result))
    {
        return false;
    }


    D3D11_BUFFER_DESC deferredBufferDesc;
    deferredBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    deferredBufferDesc.ByteWidth = sizeof(DeferredBufferType);
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


    // 행렬 상수 버퍼를 해제합니다.
    if (m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    // 레이아웃을 해제합니다.
    if (m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    // 픽셀 쉐이더를 해제합니다.
    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    // 버텍스 쉐이더를 해제합니다.
    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }
}


void DeferredShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    // 에러 메시지를 출력창에 표시합니다.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // 에러 메세지를 반환합니다.
    errorMessage->Release();
    errorMessage = 0;

    // 컴파일 에러가 있음을 팝업 메세지로 알려줍니다.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool DeferredShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
    XMFLOAT4X4 inverseProjection, XMFLOAT4X4 inverseView, int useAO, int useEnvMap,
    ID3D11ShaderResourceView* DepthTexture, ID3D11ShaderResourceView* Diffuse, ID3D11ShaderResourceView* MetalRough,
    ID3D11ShaderResourceView* Normals, ID3D11ShaderResourceView* AO,
    ID3D11ShaderResourceView* EnvMap, ID3D11ShaderResourceView* PrefilteredSpecMap, ID3D11ShaderResourceView* BrdfLUT,
    ID3D11ShaderResourceView* Emissive)
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
    unsigned int bufferNumber = 0;

    // 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);



    if (FAILED(deviceContext->Map(m_deferredBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    DeferredBufferType* dataPtr2 = (DeferredBufferType*)mappedResource.pData;

    dataPtr2->inverseProjection = inverseProjection;
    dataPtr2->inverseView = inverseView;
    dataPtr2->useAO = useAO;
    dataPtr2->useEnvMap = useEnvMap;

    deviceContext->Unmap(m_deferredBuffer, 0);

    bufferNumber = 3;

    deviceContext->PSSetConstantBuffers(bufferNumber, 3, &m_deferredBuffer);


    deviceContext->PSSetShaderResources(0, 1, &DepthTexture);
    deviceContext->PSSetShaderResources(1, 1, &Diffuse);
    deviceContext->PSSetShaderResources(2, 1, &MetalRough);
    deviceContext->PSSetShaderResources(3, 1, &Normals);
    // 4 는 섀도우 맵
    deviceContext->PSSetShaderResources(5, 1, &AO);

    deviceContext->PSSetShaderResources(6, 1, &EnvMap);
    deviceContext->PSSetShaderResources(7, 1, &PrefilteredSpecMap);
    deviceContext->PSSetShaderResources(8, 1, &BrdfLUT);
    deviceContext->PSSetShaderResources(9, 1, &Emissive);

    return true;
}


void DeferredShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // 정점 입력 레이아웃을 설정합니다.
    deviceContext->IASetInputLayout(m_layout);

    // 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // 삼각형을 그립니다.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
