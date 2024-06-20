#include "Stdafx.h"
#include "TextureShaderClass.h"
#include "LightShaderClass.h"
#include "BumpMapShaderClass.h"
#include "ShaderManagerClass.h"


ShaderManagerClass::ShaderManagerClass()
{
    m_TextureShader = 0;
    m_LightShader = 0;
    m_BumpMapShader = 0;
}


ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass& other)
{
}


ShaderManagerClass::~ShaderManagerClass()
{
}


bool ShaderManagerClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    // 텍스처 쉐이더 객체를 생성한다.
    m_TextureShader = new TextureShaderClass;
    if (!m_TextureShader)
    {
        return false;
    }

    // 텍스처 쉐이더 객체를 초기화한다.
    bool result = m_TextureShader->Initialize(device, hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
        return false;
    }

    // 라이트 쉐이더 객체를 만듭니다.
    m_LightShader = new LightShaderClass;
    if (!m_LightShader)
    {
        return false;
    }

    // 라이트 쉐이더 객체를 초기화합니다.
    result = m_LightShader->Initialize(device, hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
        return false;
    }

    // 범프 맵 쉐이더 객체를 생성합니다.
    m_BumpMapShader = new BumpMapShaderClass;
    if (!m_BumpMapShader)
    {
        return false;
    }

    // 범프 맵 쉐이더 객체를 초기화한다.
    result = m_BumpMapShader->Initialize(device, hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
        return false;
    }

    return true;
}


void ShaderManagerClass::Shutdown()
{
    // 범프 맵 쉐이더 객체를 해제한다.
    if (m_BumpMapShader)
    {
        m_BumpMapShader->Shutdown();
        delete m_BumpMapShader;
        m_BumpMapShader = 0;
    }

    // 라이트 쉐이더 객체를 해제합니다.
    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
    }

    // 텍스처 쉐이더 객체를 해제한다.
    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = 0;
    }
}


bool ShaderManagerClass::RenderTextureShader(ID3D11DeviceContext* device, int indexCount, XMMATRIX worldMatrix,
    XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
    ID3D11ShaderResourceView* texture)
{
    // 텍스처 셰이더를 사용하여 모델을 렌더링합니다.
    return m_TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}

/*
bool ShaderManagerClass::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix,
    XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,
    XMFLOAT3 lightDirection, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT3 cameraPosition,
    XMFLOAT4 specular, float specularPower)
{
    // 라이트 쉐이더를 사용하여 모델을 렌더링합니다.
    return m_LightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture,
        lightDirection, ambient, diffuse, cameraPosition, specular, specularPower);
}
*/

bool ShaderManagerClass::RenderBumpMapShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix,
    XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* colorTexture,
    ID3D11ShaderResourceView* normalTexture, XMFLOAT3 lightDirection, XMFLOAT4 diffuse)
{
    // 범프 맵 셰이더를 사용하여 모델을 렌더링합니다.
    return m_BumpMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture,
        normalTexture, lightDirection, diffuse);
}
