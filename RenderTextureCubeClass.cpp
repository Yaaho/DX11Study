#include "Stdafx.h"
#include "RenderTextureCubeClass.h"


RenderTextureCubeClass::RenderTextureCubeClass()
{
}


RenderTextureCubeClass::RenderTextureCubeClass(const RenderTextureCubeClass& other)
{
}


RenderTextureCubeClass::~RenderTextureCubeClass()
{
}


bool RenderTextureCubeClass::Initialize(ID3D11Device* device, int size)
{
	// 렌더 타겟 텍스처 설명을 초기화합니다.
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	m_CubeMapSize = size;

	// 렌더 타겟 텍스처 설명을 설정합니다.
	textureDesc.Width = size;
	textureDesc.Height = size;
	textureDesc.MipLevels = mipLevels;
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	// 렌더 타겟 텍스처를 만듭니다.
	HRESULT result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}


	int index = 0;
	for (UINT mipSlice = 0; mipSlice < mipLevels; ++mipSlice)
	{
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		renderTargetViewDesc.Texture2DArray.MipSlice = mipSlice;
		renderTargetViewDesc.Texture2DArray.ArraySize = 1;


		for (UINT i = 0; i < 6; ++i)
		{
			renderTargetViewDesc.Texture2DArray.FirstArraySlice = i;
			ID3D11RenderTargetView* rtv;
			result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &rtv);

			m_renderTargetView[index] = rtv;
			index++;
			if (FAILED(result))
			{
				return false;
			}
		}



	}


	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
	shaderResourceViewDesc.TextureCube.MipLevels = mipLevels;

	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);

	return true;
}



void RenderTextureCubeClass::Shutdown()
{
	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}

	if (m_renderTargetView)
	{
		for (int i = 0; i < 6 * mipLevels; ++i)
		{
			m_renderTargetView[i]->Release();
			m_renderTargetView[i] = 0;
		}
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}
}


void RenderTextureCubeClass::SetRenderTarget(ID3D11DeviceContext* deviceContext, int index)
{
	// 렌더링 대상 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프 라인에 바인딩합니다.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView[index], false);

	SetViewports(deviceContext);
}



void RenderTextureCubeClass::SetViewports(ID3D11DeviceContext* deviceContext)
{
	D3D11_VIEWPORT newViewport = { 0 };

	newViewport.Width = m_CubeMapSize;
	newViewport.Height = m_CubeMapSize;
	newViewport.TopLeftX = 0.0f;
	newViewport.TopLeftY = 0.0f;
	newViewport.MinDepth = 0.0f;
	newViewport.MaxDepth = 1.0f;

	// Set the viewport.
	deviceContext->RSSetViewports(1, &newViewport);
}



void RenderTextureCubeClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha)
{
	// 버퍼를 지울 색을 설정합니다.
	float color[4] = { red, green, blue, alpha };

	for (int i = 0; i < 6; i++)
	{
		deviceContext->ClearRenderTargetView(m_renderTargetView[i], color);
	}
}


ID3D11ShaderResourceView* RenderTextureCubeClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}


void RenderTextureCubeClass::UseShaderResourceView(ID3D11DeviceContext* deviceContext, int textureSlot)
{
	deviceContext->PSSetShaderResources(textureSlot, 1, &m_shaderResourceView);
}