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
    // 정점 및 픽셀 쉐이더를 초기화합니다.
    return InitializeShader(device, hwnd, L"GBufferVS.HLSL", L"GBufferPS.HLSL");
}


void GBufferShaderClass::Shutdown()
{
    // 버텍스 및 픽셀 쉐이더와 관련된 객체를 종료합니다.
    ShutdownShader();
}


bool GBufferShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
    XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
    ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* texture3, ID3D11ShaderResourceView* texture4, ID3D11ShaderResourceView* texture5,
    XMFLOAT4 gAlbedo, float gMetallic, float gRoughness, 
    int gUseAlbedoMap, int gUseOccMetalRough, int gUseAoMap, int gUseEmmisive, int gNormalState, int gConvertToLinear)
{
    // 렌더링에 사용할 셰이더 매개 변수를 설정합니다.
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, 
        texture1, texture2, texture3, texture4, texture5,
        gAlbedo, gMetallic, gRoughness,
        gUseAlbedoMap, gUseOccMetalRough, gUseAoMap, gUseEmmisive, gNormalState, gConvertToLinear))
    {
        return false;
    }

    // 설정된 버퍼를 셰이더로 렌더링한다.
    RenderShader(deviceContext, indexCount);

    return true;
}


bool GBufferShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;

    // 버텍스 쉐이더 코드를 컴파일한다.
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "GBufferVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
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
    result = D3DCompileFromFile(psFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GBufferPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
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


    // 레이아웃의 요소 수를 가져옵니다.
    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

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

    // 버텍스 쉐이더에있는 동적 행렬 상수 버퍼의 구조체를 설정합니다.
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // 이 클래스 내에서 정점 셰이더 상수 버퍼에 액세스 할 수 있도록 상수 버퍼 포인터를 만듭니다.
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

    // 이 클래스 내에서 픽셀 셰이더 상수 버퍼에 엑세스 할 수 있록 상수 버퍼 포인터를 만든다.
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


void GBufferShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    // 에러 메시지를 출력창에 표시합니다.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // 에러 메세지를 반환합니다.
    errorMessage->Release();
    errorMessage = 0;

    // 컴파일 에러가 있음을 팝업 메세지로 알려줍니다.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool GBufferShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, 
    ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* texture3, ID3D11ShaderResourceView* texture4, ID3D11ShaderResourceView* texture5,
    XMFLOAT4 gAlbedo, float gMetallic, float gRoughness,
    int gUseAlbedoMap, int gUseOccMetalRough, int gUseAoMap, int gUseEmmisive, int gNormalState, int gConvertToLinear)
{
    // 행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다
    // GPU가 열우선이기 때문에, CPU에서 보내기 전에 열우선으로 만들어서 보내주면 된다.
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


    // light constant buffer 를 기록할 수 있도록 잠근다
    if (FAILED(deviceContext->Map(m_PBRMaterialBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // constant 버퍼의 데이터에 대한 포인터를 가져온다.
    PBRMaterialBufferType* dataptr2 = (PBRMaterialBufferType*)mappedResource.pData;

    // 조명 변수를 constant 버퍼에 복사한다.
    dataptr2->gAlbedo = gAlbedo;
    dataptr2->gMetallic = gMetallic;
    dataptr2->gRoughness = gRoughness;
    dataptr2->gUseAlbedoMap = gUseAlbedoMap;
    dataptr2->gUseOccMetalRough = gUseOccMetalRough;
    dataptr2->gUseAoMap = gUseAoMap;
    dataptr2->gUseEmmisive = gUseEmmisive;
    dataptr2->gNormalState = gNormalState;
    dataptr2->gConvertToLinear = gConvertToLinear;


    // constant 버퍼의 잠금을 해제한다.
    deviceContext->Unmap(m_PBRMaterialBuffer, 0);

    // 픽셀 셰이더에서 광원 constant 버퍼의 위치를 설정한다.
    bufferNumber = 0;

    // 마지막으로 업데이트 된 값으로 픽셀 셰이더에서 광원 상수 버퍼를 설정한다.
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
    // 정점 입력 레이아웃을 설정합니다.
    deviceContext->IASetInputLayout(m_layout);

    // 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // 삼각형을 그립니다.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}