#include "Stdafx.h"
#include "RenderTexture3DClass.h"

RenderTexture3DClass::RenderTexture3DClass()
{
    
}

RenderTexture3DClass::RenderTexture3DClass(const RenderTexture3DClass& other)
{
    
}

RenderTexture3DClass::~RenderTexture3DClass()
{
    
}

bool RenderTexture3DClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, int textureDepth)
{
    D3D11_TEXTURE3D_DESC texturedesc;

    m_textureWidth = textureWidth;
    m_textureHeight = textureHeight;
    m_textureDepth = textureDepth;

    ZeroMemory(&texturedesc, sizeof(texturedesc));

    texturedesc.Width = textureWidth;
    texturedesc.Height = textureHeight;
    texturedesc.Depth = textureDepth;
    texturedesc.MipLevels = 1;
    texturedesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texturedesc.Usage = D3D11_USAGE_DEFAULT;
    texturedesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

    HRESULT result = device->CreateTexture3D(&texturedesc, NULL, &m_texture3D);
    if (FAILED(result))
    {
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = texturedesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
    shaderResourceViewDesc.Texture3D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture3D.MipLevels = texturedesc.MipLevels;

    result = device->CreateShaderResourceView(m_texture3D, &shaderResourceViewDesc, &m_shaderResourceView);
    if (FAILED(result))
    {
        return false;
    }

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    ZeroMemory(&uavDesc, sizeof(uavDesc));
    uavDesc.Format = texturedesc.Format;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
    uavDesc.Texture3D.MipSlice = 0;
    uavDesc.Texture3D.FirstWSlice = 0;
    uavDesc.Texture3D.WSize = -1; 

    result = device->CreateUnorderedAccessView(m_texture3D, &uavDesc, &m_unorderedAccessView);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

void RenderTexture3DClass::Shutdown()
{
    if (m_shaderResourceView)
    {
        m_shaderResourceView->Release();
        m_shaderResourceView = 0;
    }

    if (m_texture3D)
    {
        m_texture3D->Release();
        m_texture3D = 0;
    }
}

void RenderTexture3DClass::SetUnorderedAccessView(ID3D11DeviceContext* deviceContext)
{
    deviceContext->CSSetUnorderedAccessViews(0, 1, &m_unorderedAccessView, NULL);
}

ID3D11ShaderResourceView* RenderTexture3DClass::GetShaderResourceView()
{
    return m_shaderResourceView;
}

void RenderTexture3DClass::UsePSShaderResourceView(ID3D11DeviceContext* deviceContext, int textureSlot)
{
    deviceContext->PSSetShaderResources(textureSlot, 1, &m_shaderResourceView);
}


void RenderTexture3DClass::UseCSShaderResourceView(ID3D11DeviceContext* deviceContext, int textureSlot)
{
    deviceContext->CSSetShaderResources(textureSlot, 1, &m_shaderResourceView);
}


int RenderTexture3DClass::GetTextureWidth()
{
    return m_textureWidth;
}

int RenderTexture3DClass::GetTextureHeight()
{
    return m_textureHeight;
}

int RenderTexture3DClass::GetTextureDepth()
{
    return m_textureDepth;
}

