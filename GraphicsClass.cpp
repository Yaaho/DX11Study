#include "Stdafx.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "TextClass.h"
#include "ModelClass.h"
#include "SpecMapShaderClass.h"
#include "LightClass.h"

#include "RenderTextureClass.h"
#include "ReflectionShaderClass.h"

#include "OrthoWindowClass.h"

#include "FadeShaderClass.h"

#include "ModelListClass.h"
#include "FrustumClass.h"
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

	// m_model 객체 생성
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}
	
	// m_Model 객체 초기화
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), "data/sphere.txt") || !m_Model->LoadTextures(m_Direct3D->GetDevice(), L"data/stone02.dds", L"data/bump02.dds", L"data/spec02.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// 스패큘러 맵 셰이더 객체 생성
	m_SpecMapShader = new SpecMapShaderClass;
	if (!m_SpecMapShader)
	{
		return false;
	}

	// 스패큘러 맵 셰이더 초기화
	if (!m_SpecMapShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light map shader object.", L"Error", MB_OK);
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

	// 반사에 쓰이는 렌더 텍스쳐 객체를 생성한다.
	m_ReflectionRenderTexture = new RenderTextureClass;
	if (!m_ReflectionRenderTexture)
	{
		return false;
	}

	// 반사에 쓰이는 렌더 텍스쳐 객체를 초기화한다.
	if (!m_ReflectionRenderTexture->Initialize(m_Direct3D->GetDevice(), screenHeight, screenHeight))
	{
		return false;
	}


	m_FloorModel = new ModelClass;
	if (!m_FloorModel)
	{
		return false;
	}

	if (!m_FloorModel->Initialize(m_Direct3D->GetDevice(), "data/floor.txt") || !m_FloorModel->LoadTextures(m_Direct3D->GetDevice(), L"data/blue01.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_ReflectionShader = new ReflectionShaderClass;
	if (!m_ReflectionShader)
	{
		return false;
	}

	if (!m_ReflectionShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the reflection shader object.", L"Error", MB_OK);
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



	// 모델 목록 객체 생성
	m_ModelList = new ModelListClass;
	if (!m_ModelList)
	{
		return false;
	}

	// 모델 목록 객체 초기화
	if (!m_ModelList->Initialize(25))
	{
		MessageBox(hwnd, L"Could not initialize the model list object.", L"Error", MB_OK);
	}

	// 프러스텀 객체 생성
	m_Frustum = new FrustumClass;
	if (!m_Frustum)
	{
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// 프러스텀 객체 반환
	if (m_Frustum)
	{
		delete m_Frustum; 
		m_Frustum = 0;
	}

	// 모델 목록 객체 반환
	if (m_ModelList)
	{
		m_ModelList->Shutdown();
		delete m_ModelList;
		m_ModelList = 0;
	}

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

	// 반사 쉐이더 객체 반환
	if (m_ReflectionShader)
	{
		m_ReflectionShader->Shutdown();
		delete m_ReflectionShader;
		m_ReflectionShader = 0;
	}

	// 바닥 모델 객체 반환
	if (m_FloorModel)
	{
		m_FloorModel->Shutdown();
		delete m_FloorModel;
		m_FloorModel = 0;
	}

	// 렌더 택스쳐 객체 반환
	if (m_ReflectionRenderTexture)
	{
		m_ReflectionRenderTexture->Shutdown();
		delete m_ReflectionRenderTexture;
		m_ReflectionRenderTexture = 0;
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

	// m_AlphaMapShader 객체 반환
	if (m_SpecMapShader)
	{
		m_SpecMapShader->Shutdown();
		delete m_SpecMapShader;
		m_SpecMapShader = 0;
	}

	// m_Model 객체 반환
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
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


	// 카메라의 위치를 설정한다.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

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

	RenderToReflectionTexture();

	if (m_fadeDone)
	{
		// 페이드 인이 완료되면 백 버퍼를 사용하여 장면을 정상적으로 렌더링합니다.
		renderCount = RenderScene();
	}
	else
	{
		// 페이드 인이 완료되지 않은 경우 장면을 텍스처로 렌더링하고 텍스처를 페이드 인합니다.
		renderCount = RenderToFadeTexture();

		result = RenderFadingScene();
		if (!result)
		{
			return false;
		}
	}

	if (!RenderText(renderCount))
	{
		return false;
	}

	return true;
}




int GraphicsClass::RenderScene()
{
	float positionX = 0;
	float positionY = 0;
	float positionZ = 0;
	float radius = 1.0f; // 구의 반지름을 1.0f 로 설정
	XMFLOAT4 color;


	// 씬을 그리기 위해 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성한다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져온다.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 절두체를 만든다.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// 렌더링 될 모델의 수를 얻는다.
	int modelCount = m_ModelList->GetModelCount();

	// 렌더링 된 모델의 개수를 초기화한다.
	int renderCount = 0;

	// 모든 모델을 살펴보고 카메라 뷰에서 볼 수 있는 경우에만 렌더링한다.
	for (int index = 0; index < modelCount; index++)
	{
		// 이 인덱스에서 구형 모델의 위치와 색상을 가져온다.
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		// 구형 모델이 뷰 frustum에 있는지 확인한다.
		if (m_Frustum->CheckSphere(positionX, positionY, positionZ, radius))
		{
			// 모델을 렌더링 할 위치로 이동한다.
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);

			// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프라인에 배치하여 드로잉을 준비한다.
			m_Model->Render(m_Direct3D->GetDeviceContext());

			// 라이트 쉐이더를 사용하여 모델을 렌더링한다.
			m_SpecMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(),
				worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTextureArray(),
				m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
				m_Light->GetSpecularPower());

			// 원래의 월드 매트릭스로 리셋
			m_Direct3D->GetWorldMatrix(worldMatrix);

			// 이 모델이 렌더링 되었기 때문에 이 카운트의 수를 늘린다.
			renderCount++;
		}
	}

	m_Direct3D->GetWorldMatrix(worldMatrix);
	worldMatrix = XMMatrixTranslation(0.0f, -1.5f, 0.0f);

	XMMATRIX reflectionMatrix;
	m_Camera->RenderReflection(-1.5f, reflectionMatrix);

	m_FloorModel->Render(m_Direct3D->GetDeviceContext());

	// Render the floor model using the reflection shader, reflection texture, and reflection view matrix.
	if (!m_ReflectionShader->Render(m_Direct3D->GetDeviceContext(), m_FloorModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_FloorModel->GetTextureArray(), m_ReflectionRenderTexture->GetShaderResourceView(), reflectionMatrix))
	{
		return false;
	}

	return renderCount;
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


bool GraphicsClass::RenderToReflectionTexture()
{
	// Set the render target to be the render to texture.
	m_ReflectionRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// Clear the render to texture.
	m_ReflectionRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	float positionX = 0;
	float positionY = 0;
	float positionZ = 0;
	float radius = 1.0f; // 구의 반지름을 1.0f 로 설정
	XMFLOAT4 color;

	// 카메라의 위치에 따라 뷰 행렬을 생성한다.
	m_Camera->Render();

	XMMATRIX worldMatrix, viewMatrix, reflectionViewMatrix, projectionMatrix;
	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져온다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	// Get the camera reflection view matrix instead of the normal view matrix.
	m_Camera->RenderReflection(-1.5f, reflectionViewMatrix);

	// 절두체를 만든다.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// 렌더링 될 모델의 수를 얻는다.
	int modelCount = m_ModelList->GetModelCount();

	// 렌더링 된 모델의 개수를 초기화한다.
	int renderCount = 0;

	// 모든 모델을 살펴보고 카메라 뷰에서 볼 수 있는 경우에만 렌더링한다.
	for (int index = 0; index < modelCount; index++)
	{
		// 이 인덱스에서 구형 모델의 위치와 색상을 가져온다.
		m_ModelList->GetData(index, positionX, positionY, positionZ, color);

		// 구형 모델이 뷰 frustum에 있는지 확인한다.
		if (m_Frustum->CheckSphere(positionX, positionY, positionZ, radius))
		{
			// 모델을 렌더링 할 위치로 이동한다.
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);

			// 모델 버텍스와 인덱스 버퍼를 그래픽 파이프라인에 배치하여 드로잉을 준비한다.
			m_Model->Render(m_Direct3D->GetDeviceContext());

			// 라이트 쉐이더를 사용하여 모델을 렌더링한다.
			m_SpecMapShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(),
				worldMatrix, reflectionViewMatrix, projectionMatrix, m_Model->GetTextureArray(),
				m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(), m_Light->GetSpecularColor(),
				m_Light->GetSpecularPower());

			// 원래의 월드 매트릭스로 리셋
			m_Direct3D->GetWorldMatrix(worldMatrix);

			// 이 모델이 렌더링 되었기 때문에 이 카운트의 수를 늘린다.
			renderCount++;
		}
	}

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
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