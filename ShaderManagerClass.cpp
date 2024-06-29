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
    // ÅØ½ºÃ³ ½¦ÀÌ´õ °´Ã¼¸¦ »ý¼ºÇÑ´Ù.
    m_TextureShader = new TextureShaderClass;
    if (!m_TextureShader)
    {
        return false;
    }

    // ÅØ½ºÃ³ ½¦ÀÌ´õ °´Ã¼¸¦ ÃÊ±âÈ­ÇÑ´Ù.
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
    // ÅØ½ºÃ³ ½¦ÀÌ´õ °´Ã¼¸¦ ÇØÁ¦ÇÑ´Ù.
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
    // ÅØ½ºÃ³ ¼ÎÀÌ´õ¸¦ »ç¿ëÇÏ¿© ¸ðµ¨À» ·»´õ¸µÇÕ´Ï´Ù.
    return m_TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}