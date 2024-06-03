#include "Stdafx.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "TextClass.h"
#include "ModelClass.h"
#include "LightClass.h"
#include "RenderTextureClass.h"
#include "LightShaderClass.h"
#include "RefractionShaderClass.h"
#include "WaterShaderClass.h"
#include "OrthoWindowClass.h"
#include "FadeShaderClass.h"
#include "GraphicsClass.h"


GraphicsClass::GraphicsClass()
{

}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{

}

GraphicsClass::~GraphicsClass()
{

}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D 객체 생성
	m_Direct3D = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
	if (!m_Direct3D)
	{
		return false;
	}

	// Direct3D 객체 초기화
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, 
		FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// m_Camera 객체 생성
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}


	// 카메라의 초기 위치를 설정하고 렌더링에 필요한 행렬을 만듭니다.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->Render();

	m_Camera->RenderBaseViewMatrix();
	XMMATRIX baseViewMatrix;
	m_Camera->GetBaseViewMatrix(baseViewMatrix);

	// m_Text 객체 생성
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// m_Text 객체 초기화
	if (!m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight,
		baseViewMatrix))
	{
		MessageBox(hwnd, L"Could not initialize the next object.", L"Error", MB_OK);
		return false;
	}


	// Create the ground model object.
	m_GroundModel = new ModelClass;
	if (!m_GroundModel)
	{
		return false;
	}

	if (!m_GroundModel->Initialize(m_Direct3D->GetDevice(), "data/ground.txt") || !m_GroundModel->LoadTextures(m_Direct3D->GetDevice(), L"data/ground01.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}

	// Create the wall model object.
	m_WallModel = new ModelClass;
	if (!m_WallModel)
	{
		return false;
	}

	if (!m_WallModel->Initialize(m_Direct3D->GetDevice(), "data/wall.txt") || !m_WallModel->LoadTextures(m_Direct3D->GetDevice(), L"data/wall01.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the wall model object.", L"Error", MB_OK);
		return false;
	}

	// Create the bath model object.
	m_BathModel = new ModelClass;
	if (!m_BathModel)
	{
		return false;
	}

	if (!m_BathModel->Initialize(m_Direct3D->GetDevice(), "data/bath.txt") || !m_BathModel->LoadTextures(m_Direct3D->GetDevice(), L"data/marble01.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the bath model object.", L"Error", MB_OK);
		return false;
	}

	// Create the water model object.
	m_WaterModel = new ModelClass;
	if (!m_WaterModel)
	{
		return false;
	}

	if (!m_WaterModel->Initialize(m_Direct3D->GetDevice(), "data/water.txt") || !m_WaterModel->LoadTextures(m_Direct3D->GetDevice(), L"data/water01.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the water model object.", L"Error", MB_OK);
		return false;
	}


	// m_Light 객체 생성
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// m_Light 객체 초기화
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f);


	// 페이드에 쓰이는 렌더 텍스쳐 객체를 생성한다.
	m_FadeRenderTexture = new RenderTextureClass;
	if (!m_FadeRenderTexture)
	{
		return false;
	}

	// 페이드에 쓰이는 렌더 텍스쳐 객체를 초기화한다.
	if (!m_FadeRenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
	{
		return false;
	}

	// Create the refraction render to texture object.
	m_RefractionTexture = new RenderTextureClass;
	if (!m_RefractionTexture)
	{
		return false;
	}

	// Initialize the refraction render to texture object.
	if (!m_RefractionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
	{
		MessageBox(hwnd, L"Could not initialize the refraction render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the reflection render to texture object.
	m_ReflectionTexture = new RenderTextureClass;
	if (!m_ReflectionTexture)
	{
		return false;
	}

	// Initialize the reflection render to texture object.
	if (!m_ReflectionTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
	{
		MessageBox(hwnd, L"Could not initialize the reflection render to texture object.", L"Error", MB_OK);
		return false;
	}






	// 스패큘러 맵 셰이더 객체 생성
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// 스패큘러 맵 셰이더 초기화
	if (!m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light map shader object.", L"Error", MB_OK);
	}





	// Create the refraction shader object.
	m_RefractionShader = new RefractionShaderClass;
	if (!m_RefractionShader)
	{
		return false;
	}

	// Initialize the refraction shader object.
	if (!m_RefractionShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the refraction shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the water shader object.
	m_WaterShader = new WaterShaderClass;
	if (!m_WaterShader)
	{
		return false;
	}

	// Initialize the water shader object.
	if (!m_WaterShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the water shader object.", L"Error", MB_OK);
		return false;
	}

	// Set the height of the water.
	m_waterHeight = 2.75f;

	// Initialize the position of the water.
	m_waterTranslation = 0.0f;
	



	// 전체 화면 ortho window 객체를 생성합니다.
	m_FullScreenWindow = new OrthoWindowClass;
	if (!m_FullScreenWindow)
	{
		return false;
	}

	// 전체 화면 ortho window 객체를 초기화 합니다.
	if (!m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
	{
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}

	// 페이드 인 타임을 3000 밀리 초로 설정합니다.
	m_fadeInTime = 3000.0f;

	// 누적 된 시간을 0 밀리 초로 초기화합니다.
	m_accumulatedTime = 0;

	// 페이드 백분율을 처음에 0으로 초기화하여 장면이 검게 표시됩니다.
	m_fadePercentage = 0;

	// 효과가 사라지도록 설정합니다.
	m_fadeDone = false;

	// 페이드 셰이더 개체를 만듭니다.
	m_FadeShader = new FadeShaderClass;
	if (!m_FadeShader)
	{
		return false;
	}

	// 페이드 셰이더 개체를 초기화합니다.
	if (!m_FadeShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the fade shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	if (m_FadeShader)
	{
		m_FadeShader->Shutdown();
		delete m_FadeShader;
		m_FadeShader = 0;
	}

	if (m_FullScreenWindow)
	{
		m_FullScreenWindow->Shutdown();
		delete m_FullScreenWindow;
		m_FullScreenWindow = 0;
	}

	if (m_WaterShader)
	{
		m_WaterShader->Shutdown();
		delete m_WaterShader;
		m_WaterShader = 0;
	}

	if (m_RefractionShader)
	{
		m_RefractionShader->Shutdown();
		delete m_RefractionShader;
		m_RefractionShader = 0;
	}

	// m_AlphaMapShader 객체 반환
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// 렌더 택스쳐 객체 반환
	if (m_ReflectionTexture)
	{
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = 0;
	}

	// 렌더 택스쳐 객체 반환
	if (m_RefractionTexture)
	{
		m_RefractionTexture->Shutdown();
		delete m_RefractionTexture;
		m_RefractionTexture = 0;
	}


	// 렌더 택스쳐 객체 반환
	if (m_FadeRenderTexture)
	{
		m_FadeRenderTexture->Shutdown();
		delete m_FadeRenderTexture;
		m_FadeRenderTexture = 0;
	}



	// m_Light 객체 반환
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}



	// m_Model 객체 반환
	if (m_WaterModel)
	{
		m_WaterModel->Shutdown();
		delete m_WaterModel;
		m_WaterModel = 0;
	}

	// m_Model 객체 반환
	if (m_BathModel)
	{
		m_BathModel->Shutdown();
		delete m_BathModel;
		m_BathModel = 0;
	}

	// m_Model 객체 반환
	if (m_WallModel)
	{
		m_WallModel->Shutdown();
		delete m_WallModel;
		m_WallModel = 0;
	}

	// m_Model 객체 반환
	if (m_GroundModel)
	{
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
	}






	// m_Text 객체 반환
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// m_Camera 객체 반환
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D 객체 반환
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		_aligned_free(m_Direct3D);
		m_Direct3D = 0;
	}
}

bool GraphicsClass::Frame(float rotationY, float frameTime)
{
	if (!m_fadeDone)
	{
		// 누적 된 시간을 여분의 프레임 시간 추가로 업데이트하십시오.
		m_accumulatedTime += frameTime;

		// 시간이 갈수록 각 프레임을 통과하는 시간만큼 페이드 수가 증가합니다.
		if (m_accumulatedTime < m_fadeInTime)
		{
			// 누적 된 시간을 기준으로 화면이 희미해질 비율을 계산합니다.
			m_fadePercentage = m_accumulatedTime / m_fadeInTime;
		}
		else
		{
			// 페이드 인 타임이 완료되면 페이드 효과를 끄고 장면을 정상적으로 렌더링합니다.
			m_fadeDone = true;

			// 백분율을 100 %로 설정합니다.
			m_fadePercentage = 1.0f;
		}
	}

	// 물의 움직임을 업데이트 한다.
	m_waterTranslation += 0.001f;
	if (m_waterTranslation > 1.0f)
	{
		m_waterTranslation -= 1.0f;
	}


	// 카메라의 위치를 설정한다.
	m_Camera->SetPosition(-11.0f, 6.0f, -10.0f);

	// 카메라의 회전을 설정한다.
	m_Camera->SetRotation(0.0f, rotationY, 0.0f);


	/*
	// 초당 프레임 수를 설정한다.
	if (!m_Text->SetFps(fps, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	// cpu 사용을 설정한다.
	if (!m_Text->SetCpu(cpu, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}
	*/
	
	return true;
}

bool GraphicsClass::Render()
{
	bool result;
	int renderCount;

	RenderRefractionToTexture();
	RenderReflectionToTexture();

	if (m_fadeDone)
	{
		// 페이드 인이 완료되면 백 버퍼를 사용하여 장면을 정상적으로 렌더링합니다.
		RenderScene();
		m_Direct3D->EndScene();
	}
	else
	{
		// 페이드 인이 완료되지 않은 경우 장면을 텍스처로 렌더링하고 텍스처를 페이드 인합니다.
		RenderToFadeTexture();

		result = RenderFadingScene();
		if (!result)
		{
			return false;
		}
	}
	
	/*
	if (!RenderText(renderCount))
	{
		return false;
	}
	*/

	return true;
}




int GraphicsClass::RenderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix;

	// 장면을 시작할 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the bath model will be rendered.
	worldMatrix = XMMatrixTranslation(0.0f, 1.0f, 0.0f);

	// Put the ground model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_GroundModel->Render(m_Direct3D->GetDeviceContext());

	// Render the ground model using the light shader.
	bool result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix,
		viewMatrix, projectionMatrix, m_GroundModel->GetTextureArray(), m_Light->GetDirection(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Translate to where the wall model will be rendered.
	worldMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);

	// Put the wall model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_WallModel->Render(m_Direct3D->GetDeviceContext());

	// Render the wall model using the light shader.
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_WallModel->GetTextureArray(), m_Light->GetDirection(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Translate to where the bath model will be rendered.
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);

	// Put the bath model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_BathModel->Render(m_Direct3D->GetDeviceContext());

	// Render the bath model using the light shader.
	result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_BathModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_BathModel->GetTextureArray(), m_Light->GetDirection(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the camera reflection view matrix.
	reflectionMatrix = m_Camera->GetReflectionViewMatrix();

	// Translate to where the water model will be rendered.
	worldMatrix = XMMatrixTranslation(0.0f, m_waterHeight, 0.0f);

	// Put the water model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_WaterModel->Render(m_Direct3D->GetDeviceContext());

	// Render the water model using the water shader.
	result = m_WaterShader->Render(m_Direct3D->GetDeviceContext(), m_WaterModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, reflectionMatrix, m_ReflectionTexture->GetShaderResourceView(),
		m_RefractionTexture->GetShaderResourceView(), m_WaterModel->GetTexture(0),
		m_waterTranslation, 0.01f);
	if (!result)
	{
		return false;
	}

	return 1;
}


int GraphicsClass::RenderToFadeTexture()
{
	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_FadeRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// 렌더링을 텍스처에 지 웁니다.
	m_FadeRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	int renderCount = RenderScene();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	return renderCount;
}



bool GraphicsClass::RenderRefractionToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// 물 표면 위에 있는 것을 모두 클리핑하도록 clipPlane 설정
	XMFLOAT4 clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, m_waterHeight + 0.1f);

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_RefractionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// 렌더링을 텍스처에 지 웁니다.
	m_RefractionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f,
		0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 욕조 모델이 렌더링되는 위치로 tranlation 
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);

	// 욕조 모델의 버텍스를 렌더링할 수 있도록 정보를 그래픽 파이프라인에 배치
	m_BathModel->Render(m_Direct3D->GetDeviceContext());

	// 욕조 아래만 그려지도록 욕조를 m_RefractionShader 으로 렌더링
	if (!m_RefractionShader->Render(m_Direct3D->GetDeviceContext(), m_BathModel->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix, m_BathModel->GetTexture(0), m_Light->GetDirection(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), clipPlane))
	{
		return false;
	}

	// 렌더 타겟을 원래의 백 버퍼로 변경
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}




bool GraphicsClass::RenderReflectionToTexture()
{
	XMMATRIX reflectionViewMatrix, worldMatrix, projectionMatrix;

	// Set the render target to be the reflection render to texture.
	m_ReflectionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// Clear the reflection render to texture.
	m_ReflectionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f,
		0.0f, 1.0f);

	// Use the camera to render the reflection and create a reflection view matrix.
	m_Camera->RenderReflection(m_waterHeight);

	// Get the camera reflection view matrix instead of the normal view matrix.
	reflectionViewMatrix = m_Camera->GetReflectionViewMatrix();

	// Get the world and projection matrices from the d3d object.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Translate to where the wall model will be rendered.
	worldMatrix = XMMatrixTranslation(0.0f, 6.0f, 8.0f);

	// Put the wall model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_WallModel->Render(m_Direct3D->GetDeviceContext());

	// Render the wall model using the light shader and the reflection view matrix.
	if (!m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_WallModel->GetIndexCount(), worldMatrix,
		reflectionViewMatrix, projectionMatrix, m_WallModel->GetTextureArray(), m_Light->GetDirection(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()))
	{
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();



	return true;
}

bool GraphicsClass::RenderFadingScene()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	bool result;

	// 장면을 시작할 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 비트 맵 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// 페이드 셰이더를 사용하여 비트 맵을 렌더링합니다.
	result = m_FadeShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_FadeRenderTexture->GetShaderResourceView(), m_fadePercentage);
	if (!result)
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 된 장면을 화면에 표시합니다.
	m_Direct3D->EndScene();

	return true;
}


bool GraphicsClass::RenderText(int renderCount)
{
	XMMATRIX worldMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 이 프레임에서 실제로 렌더링 된 모델의 수를 설정한다.
	if (!m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	// 텍스트를 렌더링하기 전에 알파 블렌딩을 켠다.
	m_Direct3D->TurnOnAlphaBlending();

	// 텍스트 문자열을 렌더링 한다.
	if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
	{
		return false;
	}

	// 텍스트를 렌더링 한 후 알파 블렌딩을 해제한다.
	m_Direct3D->TurnOffAlphaBlending();

	m_Direct3D->EndScene();

	return true;
}