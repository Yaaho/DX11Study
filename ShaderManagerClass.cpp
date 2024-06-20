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

    // ����Ʈ ���̴� ��ü�� ����ϴ�.
    m_LightShader = new LightShaderClass;
    if (!m_LightShader)
    {
        return false;
    }

    // ����Ʈ ���̴� ��ü�� �ʱ�ȭ�մϴ�.
    result = m_LightShader->Initialize(device, hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
        return false;
    }

    // ���� �� ���̴� ��ü�� �����մϴ�.
    m_BumpMapShader = new BumpMapShaderClass;
    if (!m_BumpMapShader)
    {
        return false;
    }

    // ���� �� ���̴� ��ü�� �ʱ�ȭ�Ѵ�.
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
    // ���� �� ���̴� ��ü�� �����Ѵ�.
    if (m_BumpMapShader)
    {
        m_BumpMapShader->Shutdown();
        delete m_BumpMapShader;
        m_BumpMapShader = 0;
    }

    // ����Ʈ ���̴� ��ü�� �����մϴ�.
    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
    }

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

/*
bool ShaderManagerClass::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix,
    XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,
    XMFLOAT3 lightDirection, XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT3 cameraPosition,
    XMFLOAT4 specular, float specularPower)
{
    // ����Ʈ ���̴��� ����Ͽ� ���� �������մϴ�.
    return m_LightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture,
        lightDirection, ambient, diffuse, cameraPosition, specular, specularPower);
}
*/

bool ShaderManagerClass::RenderBumpMapShader(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix,
    XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* colorTexture,
    ID3D11ShaderResourceView* normalTexture, XMFLOAT3 lightDirection, XMFLOAT4 diffuse)
{
    // ���� �� ���̴��� ����Ͽ� ���� �������մϴ�.
    return m_BumpMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture,
        normalTexture, lightDirection, diffuse);
}
