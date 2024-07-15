#include "Stdafx.h"
#include "RenderTextureClass.h"

RenderTextureClass::RenderTextureClass()
{
}


RenderTextureClass::RenderTextureClass(const RenderTextureClass& other)
{
}


RenderTextureClass::~RenderTextureClass()
{
}


bool RenderTextureClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, float screenDepth, float screenNear)
{
	// 렌더 타겟 텍스처 설명을 초기화합니다.
	D3D11_TEXTURE2D_DESC textureDesc;
	// Store the width and height of the render texture.
	m_textureWidth = textureWidth;
	m_textureHeight = textureHeight;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	m_screenDepth = screenDepth;
	m_screenNear = screenNear;

	// 렌더 타겟 텍스처 설명을 설정합니다.
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

	// 렌더 타겟 텍스처를 만듭니다.
	HRESULT result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더 타겟 뷰의 설명을 설정합니다.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// 렌더 타겟 뷰를 생성한다.
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// 셰이더 리소스 뷰의 설명을 설정합니다.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// 셰이더 리소스 뷰를 만듭니다.
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the viewport for rendering.
	m_viewport.Width = (float)textureWidth;
	m_viewport.Height = (float)textureHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// Setup the projection matrix.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(((float)XM_PI / 4.0f), ((float)textureWidth / (float)textureHeight), screenNear, screenDepth);

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicLH((float)textureWidth, (float)textureHeight, screenNear, screenDepth);



	return true;
}


void RenderTextureClass::Shutdown()
{
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

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


void RenderTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext)
{
	// 렌더링 대상 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프 라인에 바인딩합니다.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	
	ResetViewports(deviceContext);
}


void RenderTextureClass::ResetViewports(ID3D11DeviceContext* deviceContext)
{
	// Set the viewport.
	deviceContext->RSSetViewports(1, &m_viewport);

	// Setup the projection matrix.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(((float)XM_PI / 4.0f), ((float)m_textureWidth / (float)m_textureHeight), m_screenNear, m_screenDepth);

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicLH((float)m_textureWidth, (float)m_textureHeight, m_screenNear, m_screenDepth);
}



void RenderTextureClass::SetViewports(ID3D11DeviceContext* deviceContext, float TopLeftX, float TopLeftY, float TextureRatio)
{
	D3D11_VIEWPORT newViewport;
	
	newViewport.TopLeftX = m_textureWidth * TopLeftX;
	newViewport.TopLeftY = m_textureHeight * TopLeftY;
	newViewport.Width = (float)(m_textureWidth * TextureRatio);
	newViewport.Height = (float)(m_textureHeight * TextureRatio);
	newViewport.MinDepth = 0.0f;
	newViewport.MaxDepth = 1.0f;

	// Set the viewport.
	deviceContext->RSSetViewports(1, &newViewport);
	

	// Setup the projection matrix.
	// 화면 비율만 구하면 되기 때문에 Ratio 변수는 넣지 않는다.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(((float)XM_PI / 4.0f), (float)m_textureWidth / (float)m_textureHeight, m_screenNear, m_screenDepth);

	// Create an orthographic projection matrix for 2D rendering.
	// 화면 비율만 구하면 되기 때문에 Radio 변수는 넣지 않는다.
	m_orthoMatrix = XMMatrixOrthographicLH((float)m_textureWidth, (float)m_textureHeight, m_screenNear, m_screenDepth);
}



void RenderTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha)
{
	// 버퍼를 지울 색을 설정합니다.
	float color[4] = { red, green, blue, alpha };

	// 백 버퍼를 지운다.
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 깊이 버퍼를 지운다.
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}


ID3D11ShaderResourceView* RenderTextureClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}


void RenderTextureClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}


void RenderTextureClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}


int RenderTextureClass::GetTextureWidth()
{
	return m_textureWidth;
}


int RenderTextureClass::GetTextureHeight()
{
	return m_textureHeight;
}