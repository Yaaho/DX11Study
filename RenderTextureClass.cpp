#include "Stdafx.h"
#include "RenderTextureClass.h"

RenderTextureClass::RenderTextureClass()
{
}

RenderTextureClass::RenderTextureClass(const RenderTextureClass&)
{
}

RenderTextureClass::~RenderTextureClass()
{
}

bool RenderTextureClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight)
{
	// 렌더 타겟 텍스쳐 설명을 초기화한다.
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// 렌더 타겟 텍스쳐 설명을 설정한다.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// 렌더 타겟 텍스쳐를 만든다.
	if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture)))
	{
		return false;
	}

	// 렌더 타겟 뷰의 설명을 설정한다.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// 렌더 타겟 뷰를 생성한다.
	if (FAILED(device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView)))
	{
		return false;
	}

	// 셰이더 리소스 뷰의 설명을 설정한다.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// 셰이더 리소스 뷰를 생성한다.
	if (FAILED(device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView)))
	{
		return false;
	}

	return true;
}


void RenderTextureClass::Shutdown()
{
	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}
}

void RenderTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// 렌더링 대상 뷰와 깊이 스텐슬 버퍼를 출력 렌더 파이프라인에 바인딩한다.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);
}

void RenderTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView,
	float red, float green, float blue, float alpha)
{
	// 버퍼를 지울 색을 설정한다.
	float color[4] = { red, green, blue, alpha };

	// 백 버퍼를 지운다.
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 깊이 버퍼를 지운다.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* RenderTextureClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}