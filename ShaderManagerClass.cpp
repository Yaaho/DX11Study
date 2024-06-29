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
    // �ؽ�ó ���̴� ��ü�� �����Ѵ�.
    m_TextureShader = new TextureShaderClass;
    if (!m_TextureShader)
    {
        return false;
    }

    // �ؽ�ó ���̴� ��ü�� �ʱ�ȭ�Ѵ�.
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
    // �ؽ�ó ���̴� ��ü�� �����Ѵ�.
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
    // �ؽ�ó ���̴��� ����Ͽ� ���� �������մϴ�.
    return m_TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}