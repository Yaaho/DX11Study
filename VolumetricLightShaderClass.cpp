#include "Stdafx.h"
#include "VolumetricLightShaderClass.h"

VolumetricLightShaderClass::VolumetricLightShaderClass()
{
}


VolumetricLightShaderClass::VolumetricLightShaderClass(const VolumetricLightShaderClass& other)
{
}


VolumetricLightShaderClass::~VolumetricLightShaderClass()
{
}


bool VolumetricLightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    // 정점 및 픽셀 쉐이더를 초기화합니다.
    return InitializeShader(device, hwnd, L"FullscreenVS.HLSL", L"VolumetricLightPS.HLSL");
}


void VolumetricLightShaderClass::Shutdown()
{
    // 버텍스 및 픽셀 쉐이더와 관련된 객체를 종료합니다.
    ShutdownShader();
}


bool VolumetricLightShaderClass::Render(ID3D11DeviceContext* deviceContext, 
    XMMATRIX View, XMMATRIX Projection, XMMATRIX invView, XMMATRIX invProjection, 
    float depthPower, float nearPlane, float farPlane,
    ID3D11ShaderResourceView* depthResource, ID3D11ShaderResourceView* voxelGrid)
{
    // 렌더링에 사용할 셰이더 매개 변수를 설정합니다.
    if (!SetShaderParameters(deviceContext, View, Projection, invView, invProjection, depthPower, nearPlane, farPlane, depthResource, voxelGrid))
    {
        return false;
    }

    // 설정된 버퍼를 셰이더로 렌더링한다.
    RenderShader(deviceContext);

    return true;
}


bool VolumetricLightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
    HRESULT result;
    ID3D10Blob* errorMessage = nullptr;
    
    // 버텍스 쉐이더 코드를 컴파일한다.
    ID3D10Blob* vertexShaderBuffer = nullptr;
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "FullscreenVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
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
    // 세이더 파일 내에 include 가 있으므로 D3D_COMPILE_STANDARD_FILE_INCLUDE 를 사용해야 한다.
    result = D3DCompileFromFile(psFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VolumetricLightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
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
    D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

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

    D3D11_BUFFER_DESC volumetricLightBufferDesc;
    volumetricLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    volumetricLightBufferDesc.ByteWidth = sizeof(VolumetricLightCBufferType);
    volumetricLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    volumetricLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    volumetricLightBufferDesc.MiscFlags = 0;
    volumetricLightBufferDesc.StructureByteStride = 0;

    result = device->CreateBuffer(&volumetricLightBufferDesc, NULL, &m_volumetricLightBuffer);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void VolumetricLightShaderClass::ShutdownShader()
{
    if (m_volumetricLightBuffer)
    {
        m_volumetricLightBuffer->Release();
        m_volumetricLightBuffer = 0;
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

void VolumetricLightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    // 에러 메시지를 출력창에 표시합니다.
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    // 에러 메세지를 반환합니다.
    errorMessage->Release();
    errorMessage = 0;

    // 컴파일 에러가 있음을 팝업 메세지로 알려줍니다.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}


bool VolumetricLightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
    XMMATRIX View, XMMATRIX Projection, XMMATRIX invView, XMMATRIX invProjection, 
    float depthPower, float nearPlane, float farPlane, ID3D11ShaderResourceView* depthResource, ID3D11ShaderResourceView* voxelGrid)
{
    // 행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다
    // GPU가 열우선이기 때문에, CPU에서 보내기 전에 열우선으로 만들어서 보내주면 된다.
    View = XMMatrixTranspose(View);
    Projection = XMMatrixTranspose(Projection);
    

    // 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    if (FAILED(deviceContext->Map(m_volumetricLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    VolumetricLightCBufferType* dataPtr = (VolumetricLightCBufferType*)mappedResource.pData;

    dataPtr->View = View;
    dataPtr->Projection = Projection;
    dataPtr->invView = invView;
    dataPtr->invProjection = invProjection;
    dataPtr->depthPower = depthPower;
    dataPtr->nearPlaneDistance = nearPlane;
    dataPtr->farPlaneDistance = farPlane;
    dataPtr->padding = 0.0f;

    deviceContext->Unmap(m_volumetricLightBuffer, 0);

    unsigned int bufferNumber = 0;

    // b0
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_volumetricLightBuffer);

    // t0
    deviceContext->PSSetShaderResources(0, 1, &depthResource);

    // t1
    deviceContext->PSSetShaderResources(1, 1, &voxelGrid);



    return true;
}

void VolumetricLightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext)
{
    // 정점 입력 레이아웃을 설정합니다.
    deviceContext->IASetInputLayout(m_layout);

    // 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // 삼각형을 그립니다.
    deviceContext->Draw(4, 0);
}



