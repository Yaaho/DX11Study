#include "Stdafx.h"
#include "TextureShaderClass.h"
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

    return true;
}


void ShaderManagerClass::Shutdown()
{
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