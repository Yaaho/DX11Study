#include "Stdafx.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "TextClass.h"
#include "ModelClass.h"

#include "LightClass.h"
#include "DepthShaderClass.h"
#include "ShadowShaderClass.h"


#include "RenderTextureClass.h"

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
	m_Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -10.0f));
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



	// 큐브 모델 오브젝트를 생성합니다.
	m_CubeModel = new ModelClass;
	if (!m_CubeModel)
	{
		return false;
	}

	if (!m_CubeModel->Initialize(m_Direct3D->GetDevice(), "data/cube.txt") || !m_CubeModel->LoadTextures(m_Direct3D->GetDevice(), L"data/wall01.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
		return false;
	}

	// 큐브 모델의 위치를 ??설정 합니다.
	m_CubeModel->SetPosition(-2.0f, 2.0f, 0.0f);


	// 지면 모델 객체를 만듭니다.
	m_GroundModel = new ModelClass;
	if (!m_GroundModel)
	{
		return false;
	}

	// 지면 모델 객체를 초기화합니다.
	if (!m_GroundModel->Initialize(m_Direct3D->GetDevice(), "data/plane01.txt") || !m_GroundModel->LoadTextures(m_Direct3D->GetDevice(), L"data/metal001.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}

	// 지면 모델의 위치를 ??설정합니다.
	m_GroundModel->SetPosition(0.0f, 1.0f, 0.0f);


	// 구형 모델 객체를 만듭니다.
	m_SphereModel = new ModelClass;
	if (!m_SphereModel)
	{
		return false;
	}

	// 구형 모델 객체를 초기화합니다.
	if (!m_SphereModel->Initialize(m_Direct3D->GetDevice(), "data/sphere.txt") || !m_SphereModel->LoadTextures(m_Direct3D->GetDevice(), L"data/ice.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK);
		return false;
	}

	// 구형 모델의 위치를 ??설정합니다.
	m_SphereModel->SetPosition(2.0f, 2.0f, 0.0f);



	// light 객체를 만듭니다.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// 조명 객체를 초기화합니다.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);


	// 깊이 셰이더 개체를 만듭니다.
	m_DepthShader = new DepthShaderClass;
	if (!m_DepthShader)
	{
		return false;
	}

	// 깊이 셰이더 개체를 초기화합니다.
	if (!m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return false;
	}

	// 그림자 셰이더 개체를 만듭니다.
	m_ShadowShader = new ShadowShaderClass;
	if (!m_ShadowShader)
	{
		return false;
	}

	// 그림자 쉐이더 객체를 초기화합니다.
	if (!m_ShadowShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}



	// 렌더링을 텍스처 오브젝트에 생성한다.
	m_DepthMapTexture = new RenderTextureClass;
	if (!m_DepthMapTexture)
	{
		return false;
	}

	// 렌더링을 텍스처 오브젝트에 초기화한다.
	if (!m_DepthMapTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT,
		SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}




	// 페이드에 쓰이는 렌더 텍스쳐 객체를 생성한다.
	m_FadeRenderTexture = new RenderTextureClass;
	if (!m_FadeRenderTexture)
	{
		return false;
	}

	// 페이드에 쓰이는 렌더 텍스쳐 객체를 초기화한다.
	if (!m_FadeRenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR))
	{
		return false;
	}



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

	// 렌더 택스쳐 객체 반환
	if (m_FadeRenderTexture)
	{
		m_FadeRenderTexture->Shutdown();
		delete m_FadeRenderTexture;
		m_FadeRenderTexture = 0;
	}


	// 렌더 투 텍스쳐 객체를 해제합니다.
	if (m_DepthMapTexture)
	{
		m_DepthMapTexture->Shutdown();
		delete m_DepthMapTexture;
		m_DepthMapTexture = 0;
	}




	// 그림자 쉐이더 객체를 해제합니다.
	if (m_ShadowShader)
	{
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	}



	// 깊이 셰이더 개체를 해제합니다.
	if (m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}




	// 조명 객체를 해제합니다.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}



	// 구형 모델 객체를 해제합니다.
	if (m_SphereModel)
	{
		m_SphereModel->Shutdown();
		delete m_SphereModel;
		m_SphereModel = 0;
	}


	// 지면 모델 객체를 해제합니다.
	if (m_GroundModel)
	{
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
	}


	// 큐브 모델 객체를 해제합니다.
	if (m_CubeModel)
	{
		m_CubeModel->Shutdown();
		delete m_CubeModel;
		m_CubeModel = 0;
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

bool GraphicsClass::Frame(float frameTime, float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
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


	static float lightPositionX = -5.0f;

	// 카메라 위치를 설정합니다.
	m_Camera->SetPosition(XMFLOAT3(posX, posY, posZ));
	m_Camera->SetRotation(XMFLOAT3(rotX, rotY, rotZ));

	// 각 프레임의 조명 위치를 업데이트합니다.
	lightPositionX += 0.05f;
	if (lightPositionX > 5.0f)
	{
		lightPositionX = -5.0f;
	}

	// 빛의 위치를 ??업데이트합니다.
	m_Light->SetPosition(lightPositionX, 8.0f, -5.0f);


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
	bool result = false;

	RenderToDepthMapTexture();

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

		RenderFadingScene();
	}


	/*
	if (!RenderText(renderCount))
	{
		return false;
	}
	*/

	return true;
}


bool GraphicsClass::RenderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;

	float posX = 0;
	float posY = 0;
	float posZ = 0;


	// 장면을 시작할 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_Light->GenerateViewMatrix();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 라이트 오브젝트로부터 라이트의 뷰와 투영 행렬을 가져옵니다.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// 큐브 모델에 대한 변환 행렬을 설정하십시오.
	m_CubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 큐브 모델 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	m_CubeModel->Render(m_Direct3D->GetDeviceContext());

	// 그림자 쉐이더를 사용하여 모델을 렌더링합니다.
	if (!m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_CubeModel->GetTexture(0),
		m_DepthMapTexture->GetShaderResourceView(), m_Light->GetPosition(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()))
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 구형 모델에 대한 변환 행렬을 설정합니다.
	m_SphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
	m_SphereModel->Render(m_Direct3D->GetDeviceContext());

	if (!m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_SphereModel->GetTexture(0),
		m_DepthMapTexture->GetShaderResourceView(), m_Light->GetPosition(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()))
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground 모델에 대한 변환 행렬을 설정합니다.
	m_GroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 그림자 쉐이더를 사용하여 그라운드 모델을 렌더링합니다.
	m_GroundModel->Render(m_Direct3D->GetDeviceContext());

	if (!m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_GroundModel->GetTexture(0),
		m_DepthMapTexture->GetShaderResourceView(), m_Light->GetPosition(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()))
	{
		return false;
	}


	return true;
}


bool GraphicsClass::RenderToDepthMapTexture()
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix;

	float posX = 0;
	float posY = 0;
	float posZ = 0;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_DepthMapTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_DepthMapTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 조명의 위치에 따라 조명보기 행렬을 생성합니다.
	m_Light->GenerateViewMatrix();

	// d3d 객체에서 세계 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 라이트 오브젝트에서 뷰 및 정사각형 매트릭스를 가져옵니다.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// 큐브 모델에 대한 변환 행렬을 설정하십시오.
	m_CubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 큐브 모델을 렌더링합니다.
	m_CubeModel->Render(m_Direct3D->GetDeviceContext());
	bool result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix,
		lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 구형 모델에 대한 변환 행렬을 설정합니다.
	m_SphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 구형 모델을 렌더링합니다.
	m_SphereModel->Render(m_Direct3D->GetDeviceContext());
	result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix,
		lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// 월드 행렬을 재설정합니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground 모델에 대한 변환 행렬을 설정합니다.
	m_GroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// 깊이 셰이더로 그라운드 모델을 렌더링합니다.
	m_GroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix,
		lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}



bool GraphicsClass::RenderToFadeTexture()
{
	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_FadeRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지 웁니다.
	m_FadeRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	bool Result = RenderScene();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	return Result;
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
	m_Direct3D->EnableAlphaBlending();

	// 텍스트 문자열을 렌더링 한다.
	if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
	{
		return false;
	}

	// 텍스트를 렌더링 한 후 알파 블렌딩을 해제한다.
	m_Direct3D->DisableAlphaBlending();

	m_Direct3D->EndScene();

	return true;
}
