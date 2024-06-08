#include "Stdafx.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "TextClass.h"
#include "ModelClass.h"


#include "FireShaderClass.h"

#include "TextureShaderClass.h"


#include "HorizontalblurShaderClass.h"
#include "VerticalblurShaderClass.h"


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
	m_Camera->SetPosition(XMFLOAT3(0.0f, 2.0f, -10.0f));
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



	// 모델 객체 생성
	m_FireModel = new ModelClass;
	if (!m_FireModel)
	{
		return false;
	}

	// 모델 객체 초기화
	if (!m_FireModel->Initialize(m_Direct3D->GetDevice(), "data/square.txt") || !m_FireModel->LoadTextures(m_Direct3D->GetDevice(),
		L"data/fire01.dds", L"data/noise01.dds", L"data/alpha01.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}



	m_FireShader = new FireShaderClass;
	if (!m_FireShader)
	{
		return false;
	}

	// 텍스처 쉐이더 객체를 초기화한다.
	if (!m_FireShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the fire shader object.", L"Error", MB_OK);
		return false;
	}

	

	// 텍스처 쉐이더 객체를 생성한다.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// 텍스처 쉐이더 객체를 초기화한다.
	if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}




	// 수평 블러 쉐이더 객체를 만듭니다.
	m_HorizontalblurShader = new HorizontalblurShaderClass;
	if (!m_HorizontalblurShader)
	{
		return false;
	}

	// 수평 블러 쉐이더 객체를 초기화합니다.
	if (!m_HorizontalblurShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur shader object.", L"Error", MB_OK);
		return false;
	}

	// 수직 블러 셰이더 오브젝트를 생성한다.
	m_VerticalblurShader = new VerticalblurShaderClass;
	if (!m_VerticalblurShader)
	{
		return false;
	}

	// 수직 블러 쉐이더 객체를 초기화합니다.
	if (!m_VerticalblurShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur shader object.", L"Error", MB_OK);
		return false;
	}




	// 크기를 샘플로 설정합니다.
	int downSampleWidth = screenWidth / 2;
	int downSampleHeight = screenHeight / 2;


	// 렌더링 텍스처 객체를 생성한다.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// 렌더링 텍스처 객체를 초기화한다.
	if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}


	// 다운 샘플 렌더링을 텍스처 오브젝트에 생성한다.
	m_DownSampleTexure = new RenderTextureClass;
	if (!m_DownSampleTexure)
	{
		return false;
	}

	// 다운 샘플 렌더를 텍스처 오브젝트로 초기화한다.
	if (!m_DownSampleTexure->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH,
		SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the down sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 텍스처 객체에 수평 블러 렌더링을 만듭니다.
	m_HorizontalBlurTexture = new RenderTextureClass;
	if (!m_HorizontalBlurTexture)
	{
		return false;
	}

	// 텍스처 객체에 수평 블러 렌더링을 초기화합니다.
	if (!m_HorizontalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH,
		SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 텍스처 오브젝트에 수직 블러 렌더를 만듭니다.
	m_VerticalBlurTexture = new RenderTextureClass;
	if (!m_VerticalBlurTexture)
	{
		return false;
	}

	// 텍스처 오브젝트에 수직 블러 렌더를 초기화합니다.
	if (!m_VerticalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH,
		SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// 텍스쳐 객체에 업 샘플 렌더를 생성한다.
	m_UpSampleTexure = new RenderTextureClass;
	if (!m_UpSampleTexure)
	{
		return false;
	}

	// up 샘플 렌더를 텍스처 오브젝트로 초기화한다.
	if (!m_UpSampleTexure->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the up sample render to texture object.", L"Error", MB_OK);
		return false;
	}




	/*
	// m_Light 객체 초기화
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f);
	*/


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


	// 작은 ortho 윈도우 객체를 만듭니다.
	m_SmallWindow = new OrthoWindowClass;
	if (!m_SmallWindow)
	{
		return false;
	}

	// 작은 ortho 윈도우 객체를 초기화합니다.
	if (!m_SmallWindow->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight))
	{
		MessageBox(hwnd, L"Could not initialize the small ortho window object.", L"Error", MB_OK);
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
	if (m_SmallWindow)
	{
		m_SmallWindow->Shutdown();
		delete m_SmallWindow;
		m_SmallWindow = 0;
	}


	// 렌더 택스쳐 객체 반환
	if (m_FadeRenderTexture)
	{
		m_FadeRenderTexture->Shutdown();
		delete m_FadeRenderTexture;
		m_FadeRenderTexture = 0;
	}

	/*
	// m_Light 객체 반환
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}
	*/



	if (m_UpSampleTexure)
	{
		m_UpSampleTexure->Shutdown();
		delete m_UpSampleTexure;
		m_UpSampleTexure = 0;
	}

	if (m_VerticalBlurTexture)
	{
		m_VerticalBlurTexture->Shutdown();
		delete m_VerticalBlurTexture;
		m_VerticalBlurTexture = 0;
	}

	if (m_HorizontalBlurTexture)
	{
		m_HorizontalBlurTexture->Shutdown();
		delete m_HorizontalBlurTexture;
		m_HorizontalBlurTexture = 0;
	}

	if (m_DownSampleTexure)
	{
		m_DownSampleTexure->Shutdown();
		delete m_DownSampleTexure;
		m_DownSampleTexure = 0;
	}


	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}


	if (m_VerticalblurShader)
	{
		m_VerticalblurShader->Shutdown();
		delete m_VerticalblurShader;
		m_VerticalblurShader = 0;
	}



	if (m_HorizontalblurShader)
	{
		m_HorizontalblurShader->Shutdown();
		delete m_HorizontalblurShader;
		m_HorizontalblurShader = 0;
	}


	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}



	// m_FireShader 객체 반환
	if (m_FireShader)
	{
		m_FireShader->Shutdown();
		delete m_FireShader;
		m_FireShader = 0;
	}

	// 모델 객체를 해제합니다.
	if (m_FireModel)
	{
		m_FireModel->Shutdown();
		delete m_FireModel;
		m_FireModel = 0;
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

bool GraphicsClass::Frame(XMFLOAT3& rotation, float frameTime)
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
	m_Camera->SetRotation(rotation);


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

	// 전체 장면을 먼저 텍스처로 렌더링합니다.
	result = RenderSceneToTexture();
	if (!result)
	{
		return false;
	}

	// 다음으로 렌더 텍스처를 작은 크기의 텍스쳐로 샘플링한다.
	result = DownSampleTexture();
	if (!result)
	{
		return false;
	}

	// 다운 샘플링 된 렌더링 텍스처에 수평 블러를 수행한다.
	result = RenderHorizontalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// 수평 블러된 렌더 텍스처에서 수직 블러를 수행합니다.
	result = RenderVerticalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// 최종 흐린 렌더링 텍스처를 화면 크기로 다시 샘플링한다.
	result = UpSampleTexture();
	if (!result)
	{
		return false;
	}

	// 흐리게 샘플링 된 렌더링 텍스처를 화면에 렌더링합니다.
	result = Render2DTextureScene();
	if (!result)
	{
		return false;
	}





	/*
	int renderCount;

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
	*/
	
	/*
	if (!RenderText(renderCount))
	{
		return false;
	}

	*/

	return true;
}


bool GraphicsClass::RenderSceneToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 빌보드 설정
	XMMATRIX translateMatrix;
	XMFLOAT3 cameraPosition, modelPosition;

	// 카메라 위치를 얻는다.
	cameraPosition = m_Camera->GetPosition();

	// 빌보드 모델의 위치를 설정한다.
	modelPosition.x = 0.0f;
	modelPosition.y = 1.0f;
	modelPosition.z = 3.0f;

	// 아크 탄젠트 함수를 사용하여 현재 카메라 위치를 향하도록 빌보드 모델에 적용해야하는 회전을 계산합니다.
	double angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0 / XM_PI);

	// 회전을 라디안으로 변환합니다.
	float rotation = (float)angle * 0.0174532925f;

	// 월드 행렬을 이용하여 원점에서 빌보드 회전을 설정합니다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 빌보드 모델의 위치를 설정한다.
	translateMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

	// 마지막으로 rotation 및 Translation 행렬을 결합하여 빌보드 모델의 최종 행렬을 만듭니다.
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);




	// 불 셰이더 설정
	static float frameTime = 0.0f;

	// 프레임 시간 카운터를 증가시킵니다.
	frameTime += 0.01f;
	if (frameTime > 1000.0f)
	{
		frameTime = 0.0f;
	}

	// 세 가지 다른 노이즈 텍스처에 대해 세 가지 스크롤 속도를 설정합니다.
	XMFLOAT3 scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f);

	// 세 개의 서로 다른 노이즈 옥타브 텍스처를 만드는 데 사용할 세 개의 스케일을 설정합니다.
	XMFLOAT3 scales = XMFLOAT3(1.0f, 2.0f, 3.0f);

	// 세 가지 다른 노이즈 텍스처에 대해 세 가지 다른 x 및 y 왜곡 인수를 설정합니다.
	XMFLOAT2 distortion1 = XMFLOAT2(0.1f, 0.2f);
	XMFLOAT2 distortion2 = XMFLOAT2(0.1f, 0.3f);
	XMFLOAT2 distortion3 = XMFLOAT2(0.1f, 0.1f);

	// 텍스처 좌표 샘플링 섭동의 스케일과 바이어스.
	float distortionScale = 0.8f;
	float distortionBias = 0.5f;






	// 화재 투명도의 알파 블렌드를 켭니다.
	m_Direct3D->TurnOnAlphaBlending();

	// 정사각형 모델의 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	m_FireModel->Render(m_Direct3D->GetDeviceContext());

	// 화재 쉐이더를 사용하여 사각형 모델을 렌더링합니다.
	if (!m_FireShader->Render(m_Direct3D->GetDeviceContext(), m_FireModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_FireModel->GetTexture(0), m_FireModel->GetTexture(1), m_FireModel->GetTexture(2),
		frameTime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale,
		distortionBias))
	{
		return false;
	}

	// 알파 블렌딩을 끕니다.
	m_Direct3D->TurnOffAlphaBlending();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}


bool GraphicsClass::DownSampleTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_DownSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_DownSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 질감이 크기가 작기 때문에 렌더링에서 텍스처로 ortho 행렬을 가져옵니다.
	m_DownSampleTexure->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 드로잉을 준비하기 위해 그래픽 파이프 라인에 작은 ortho window 버텍스와 인덱스 버퍼를 놓습니다.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더를 사용하여 작은 ortho 창을 렌더링하고 씬의 텍스처를 텍스처 리소스로 렌더링합니다.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix,
		orthoMatrix, m_RenderTexture->GetShaderResourceView()))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}




bool GraphicsClass::RenderHorizontalBlurToTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// 수평 블러 쉐이더에서 사용될 float에 화면 폭을 저장합니다.
	float screenSizeX = (float)m_HorizontalBlurTexture->GetTextureWidth();

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_HorizontalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_HorizontalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 텍스쳐가 다른 차원을 가지므로 렌더링에서 오쏘 (ortho) 행렬을 텍스처로 가져옵니다.
	m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 드로잉을 준비하기 위해 그래픽 파이프 라인에 작은 ortho window 버텍스와 인덱스 버퍼를 놓습니다.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// horizontal blur shader와 down sampled render를 사용하여 작은 ortho 윈도우를 텍스처 리소스로 렌더링합니다.    
	if (!m_HorizontalblurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix,
		baseViewMatrix, orthoMatrix, m_DownSampleTexure->GetShaderResourceView(), screenSizeX))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderVerticalBlurToTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// 수직 블러 셰이더에서 사용되는 부동 소수점에 화면 높이를 저장합니다.
	float screenSizeY = (float)m_VerticalBlurTexture->GetTextureHeight();

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_VerticalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_VerticalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 텍스쳐가 다른 차원을 가지므로 렌더링에서 오쏘 (ortho) 행렬을 텍스처로 가져옵니다.
	m_VerticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 드로잉을 준비하기 위해 그래픽 파이프 라인에 작은 ortho window 버텍스와 인덱스 버퍼를 놓습니다.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// 수직 블러 쉐이더와 수평 블러 링을 사용하여 작은 ortho 윈도우를 텍스처 리소스로 렌더합니다.
	if (!m_VerticalblurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix,
		baseViewMatrix, orthoMatrix, m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::UpSampleTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_UpSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지웁니다.
	m_UpSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라와 d3d 객체로부터 월드와 뷰 매트릭스를 얻는다.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// 텍스쳐가 다른 차원을 가지므로 렌더링에서 오쏘 (ortho) 행렬을 텍스처로 가져옵니다.
	m_UpSampleTexure->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 그래픽 파이프 라인에 전체 화면 직교 윈도우 버텍스와 인덱스 버퍼를 배치하여 그리기를 준비합니다.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더와 텍스처 리소스에 대한 작은 크기의 최종 흐리게 렌더링을 사용하여 전체 화면 ortho 창을 렌더링합니다.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix,
		baseViewMatrix, orthoMatrix, m_VerticalBlurTexture->GetShaderResourceView()))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	// 뷰포트를 원본으로 다시 설정합니다.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::Render2DTextureScene()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// 장면을 시작할 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 0.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 그래픽 파이프 라인에 전체 화면 직교 윈도우 버텍스와 인덱스 버퍼를 배치하여 그리기를 준비합니다.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// 텍스처 쉐이더를 사용하여 전체 화면 ortho 창을 렌더링하고 텍스처 리소스에 전체 화면 크기로 흐리게 렌더링합니다.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix,
		baseViewMatrix, orthoMatrix, m_UpSampleTexure->GetShaderResourceView()))
	{
		return false;
	}

	// 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
	m_Direct3D->TurnZBufferOn();

	// 렌더링 된 장면을 화면에 표시합니다.
	m_Direct3D->EndScene();

	return true;
}














/*
int GraphicsClass::RenderScene()
{

	// 장면을 시작할 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성합니다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 오쏘 (ortho) 행렬을 가져옵니다.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	
	// 빌보드 설정
	XMMATRIX translateMatrix;
	XMFLOAT3 cameraPosition, modelPosition;

	// 카메라 위치를 얻는다.
	cameraPosition = m_Camera->GetPosition();

	// 빌보드 모델의 위치를 설정한다.
	modelPosition.x = 0.0f;
	modelPosition.y = 1.0f;
	modelPosition.z = 3.0f;

	// 아크 탄젠트 함수를 사용하여 현재 카메라 위치를 향하도록 빌보드 모델에 적용해야하는 회전을 계산합니다.
	double angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0 / XM_PI);

	// 회전을 라디안으로 변환합니다.
	float rotation = (float)angle * 0.0174532925f;

	// 월드 행렬을 이용하여 원점에서 빌보드 회전을 설정합니다.
	worldMatrix = XMMatrixRotationY(rotation);

	// 빌보드 모델의 위치를 설정한다.
	translateMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

	// 마지막으로 rotation 및 Translation 행렬을 결합하여 빌보드 모델의 최종 행렬을 만듭니다.
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);




	// 불 셰이더 설정
	static float frameTime = 0.0f;

	// 프레임 시간 카운터를 증가시킵니다.
	frameTime += 0.01f;
	if (frameTime > 1000.0f)
	{
		frameTime = 0.0f;
	}

	// 세 가지 다른 노이즈 텍스처에 대해 세 가지 스크롤 속도를 설정합니다.
	XMFLOAT3 scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f);

	// 세 개의 서로 다른 노이즈 옥타브 텍스처를 만드는 데 사용할 세 개의 스케일을 설정합니다.
	XMFLOAT3 scales = XMFLOAT3(1.0f, 2.0f, 3.0f);

	// 세 가지 다른 노이즈 텍스처에 대해 세 가지 다른 x 및 y 왜곡 인수를 설정합니다.
	XMFLOAT2 distortion1 = XMFLOAT2(0.1f, 0.2f);
	XMFLOAT2 distortion2 = XMFLOAT2(0.1f, 0.3f);
	XMFLOAT2 distortion3 = XMFLOAT2(0.1f, 0.1f);

	// 텍스처 좌표 샘플링 섭동의 스케일과 바이어스.
	float distortionScale = 0.8f;
	float distortionBias = 0.5f;






	// 화재 투명도의 알파 블렌드를 켭니다.
	m_Direct3D->TurnOnAlphaBlending();

	// 정사각형 모델의 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
	m_FireModel->Render(m_Direct3D->GetDeviceContext());

	// 화재 쉐이더를 사용하여 사각형 모델을 렌더링합니다.
	if (!m_FireShader->Render(m_Direct3D->GetDeviceContext(), m_FireModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_FireModel->GetTexture(0), m_FireModel->GetTexture(1), m_FireModel->GetTexture(2),
		frameTime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale,
		distortionBias))
	{
		return false;
	}

	// 알파 블렌딩을 끕니다.
	m_Direct3D->TurnOffAlphaBlending();

	return 1;
}


int GraphicsClass::RenderToFadeTexture()
{
	// 렌더링 대상을 렌더링에 맞게 설정합니다.
	m_FadeRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 렌더링을 텍스처에 지 웁니다.
	m_FadeRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	int renderCount = RenderScene();

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
	m_Direct3D->SetBackBufferRenderTarget();

	return renderCount;
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
*/

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
