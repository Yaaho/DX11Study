#include "Stdafx.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "TextClass.h"
#include "ModelClass.h"
#include "SpecMapShaderClass.h"
#include "LightClass.h"

#include "RenderTextureClass.h"
#include "DebugWindowClass.h"
#include "TextureShaderClass.h"

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

	// 카메라 포지션 설정
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

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
	if (!m_Model->Initialize(m_Direct3D->GetDevice(), "data/sphere.txt", 
		L"data/stone02.dds", L"data/bump02.dds", L"data/spec02.dds"))
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

	// 렌더 텍스쳐 객체를 생성한다.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// 렌더 텍스쳐 객체를 초기화한다.
	if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
	{
		return false;
	}

	// 디버그 창 객체를 만든다.
	m_DebugWindow = new DebugWindowClass;
	if (!m_DebugWindow)
	{
		return false;
	}

	// 디버그 창 객체를 초기화한다.
	if (!m_DebugWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, 100, 100))
	{
		MessageBox(hwnd, L"Could not initialize the debug window object", L"Error", MB_OK);
		return false;
	}

	// 텍스쳐 셰이더 객체를 생성한다.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// 텍스쳐 셰이더 객체를 초기화한다.
	if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object", L"Error", MB_OK);
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

	// 텍스쳐 쉐이더 객체 반환
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// 디버그 창 객체 반환
	if (m_DebugWindow)
	{
		m_DebugWindow->Shutdown();
		delete m_DebugWindow;
		m_DebugWindow = 0;
	}

	// 렌더 택스쳐 객체 반환
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
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

bool GraphicsClass::Frame(float rotationY)
{
	// 카메라의 위치를 설정한다.
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

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
	// 전체 장면을 먼저 텍스쳐로 렌더링한다.
	if (!RenderToTexture())
	{
		return false;
	}

	// 씬을 그리기 위해 버퍼를 지웁니다
	// m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 백 버퍼의 장면을 정상적으로 렌더링한다.
	if (!RenerScene())
	{
		return false;
	}

	// 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
	m_Direct3D->TurnZBufferOff();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져온다.
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	m_Camera->GetDebugViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 디버그 윈도우 버텍스와 인덱스 버퍼를 그래픽 파이프라인에 배치하여 그리기를 준비한다.
	if (!m_DebugWindow->Render(m_Direct3D->GetDeviceContext(), 50, 50))
	{
		return false;
	}

	// 디버그 윈도우 버텍스와 인덱스 버퍼를 그래픽 파이프라인에 배치하여 그리기를 준비한다.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_DebugWindow->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_RenderTexture->GetShaderResourceView()))
	{
		return false;
	}

	// 모든 2d 렌더링이 완료되었으므로 Z 버퍼를 다시 켠다.
	m_Direct3D->TurnZBufferOn();

	// 버퍼의 내용을 화면에 출력합니다
	m_Direct3D->EndScene();

	return true;
}


bool GraphicsClass::RenderToTexture()
{
	// 렌더 타겟을 렌더링에 맞게 설정한다.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// 렌더 타겟의 색상을 초기화한다.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(),
		0.0f, 0.0f, 1.0f, 1.0f);

	//  백 버퍼 대신 텍스쳐가 렌더링된다.
	if (!RenerScene())
	{
		return false;
	}

	// 렌더링 대상을 원래의 백 버퍼로 다시 설정한다.
	m_Direct3D->SetBackBufferRenderTarget();

	return true;
}

bool GraphicsClass::RenerScene()
{
	float positionX = 0;
	float positionY = 0;
	float positionZ = 0;
	float radius = 1.0f; // 구의 반지름을 1.0f 로 설정
	XMFLOAT4 color;

	float blendAmount = 0.5f;

	// 씬을 그리기 위해 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라의 위치에 따라 뷰 행렬을 생성한다.
	m_Camera->Render();

	// 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져온다.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// 절두체를 만든다.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

	// 투명도가 작동하도록 알파 블렌딩을 킨다.
	m_Direct3D->TurnOnAlphaBlending();

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
				m_Light->GetSpecularPower(), blendAmount);

			// 원래의 월드 매트릭스로 리셋
			m_Direct3D->GetWorldMatrix(worldMatrix);

			// 이 모델이 렌더링 되었기 때문에 이 카운트의 수를 늘린다.
			renderCount++;
		}
	}

	// 이 프레임에서 실제로 렌더링 된 모델의 수를 설정한다.
	if (!m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	// 텍스트를 렌더링하기 정에 알파 블렌딩을 켠다.
	m_Direct3D->TurnOnAlphaBlending();

	// 텍스트 문자열을 렌더링 한다.
	if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
	{
		return false;
	}

	// 텍스트를 렌더링 한 후 알파 블렌딩을 해제한다.
	m_Direct3D->TurnOffAlphaBlending();

	return true;
}