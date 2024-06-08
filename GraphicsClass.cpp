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
	// Direct3D ��ü ����
	m_Direct3D = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
	if (!m_Direct3D)
	{
		return false;
	}

	// Direct3D ��ü �ʱ�ȭ
	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, 
		FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// m_Camera ��ü ����
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// ī�޶��� �ʱ� ��ġ�� �����ϰ� �������� �ʿ��� ����� ����ϴ�.
	m_Camera->SetPosition(XMFLOAT3(0.0f, 2.0f, -10.0f));
	m_Camera->Render();

	m_Camera->RenderBaseViewMatrix();
	XMMATRIX baseViewMatrix;
	m_Camera->GetBaseViewMatrix(baseViewMatrix);

	// m_Text ��ü ����
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// m_Text ��ü �ʱ�ȭ
	if (!m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight,
		baseViewMatrix))
	{
		MessageBox(hwnd, L"Could not initialize the next object.", L"Error", MB_OK);
		return false;
	}



	// �� ��ü ����
	m_FireModel = new ModelClass;
	if (!m_FireModel)
	{
		return false;
	}

	// �� ��ü �ʱ�ȭ
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

	// �ؽ�ó ���̴� ��ü�� �ʱ�ȭ�Ѵ�.
	if (!m_FireShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the fire shader object.", L"Error", MB_OK);
		return false;
	}

	

	// �ؽ�ó ���̴� ��ü�� �����Ѵ�.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// �ؽ�ó ���̴� ��ü�� �ʱ�ȭ�Ѵ�.
	if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}




	// ���� �� ���̴� ��ü�� ����ϴ�.
	m_HorizontalblurShader = new HorizontalblurShaderClass;
	if (!m_HorizontalblurShader)
	{
		return false;
	}

	// ���� �� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_HorizontalblurShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur shader object.", L"Error", MB_OK);
		return false;
	}

	// ���� �� ���̴� ������Ʈ�� �����Ѵ�.
	m_VerticalblurShader = new VerticalblurShaderClass;
	if (!m_VerticalblurShader)
	{
		return false;
	}

	// ���� �� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_VerticalblurShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur shader object.", L"Error", MB_OK);
		return false;
	}




	// ũ�⸦ ���÷� �����մϴ�.
	int downSampleWidth = screenWidth / 2;
	int downSampleHeight = screenHeight / 2;


	// ������ �ؽ�ó ��ü�� �����Ѵ�.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// ������ �ؽ�ó ��ü�� �ʱ�ȭ�Ѵ�.
	if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}


	// �ٿ� ���� �������� �ؽ�ó ������Ʈ�� �����Ѵ�.
	m_DownSampleTexure = new RenderTextureClass;
	if (!m_DownSampleTexure)
	{
		return false;
	}

	// �ٿ� ���� ������ �ؽ�ó ������Ʈ�� �ʱ�ȭ�Ѵ�.
	if (!m_DownSampleTexure->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH,
		SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the down sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// �ؽ�ó ��ü�� ���� �� �������� ����ϴ�.
	m_HorizontalBlurTexture = new RenderTextureClass;
	if (!m_HorizontalBlurTexture)
	{
		return false;
	}

	// �ؽ�ó ��ü�� ���� �� �������� �ʱ�ȭ�մϴ�.
	if (!m_HorizontalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH,
		SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// �ؽ�ó ������Ʈ�� ���� �� ������ ����ϴ�.
	m_VerticalBlurTexture = new RenderTextureClass;
	if (!m_VerticalBlurTexture)
	{
		return false;
	}

	// �ؽ�ó ������Ʈ�� ���� �� ������ �ʱ�ȭ�մϴ�.
	if (!m_VerticalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH,
		SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// �ؽ��� ��ü�� �� ���� ������ �����Ѵ�.
	m_UpSampleTexure = new RenderTextureClass;
	if (!m_UpSampleTexure)
	{
		return false;
	}

	// up ���� ������ �ؽ�ó ������Ʈ�� �ʱ�ȭ�Ѵ�.
	if (!m_UpSampleTexure->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the up sample render to texture object.", L"Error", MB_OK);
		return false;
	}




	/*
	// m_Light ��ü �ʱ�ȭ
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f);
	*/


	// ���̵忡 ���̴� ���� �ؽ��� ��ü�� �����Ѵ�.
	m_FadeRenderTexture = new RenderTextureClass;
	if (!m_FadeRenderTexture)
	{
		return false;
	}

	// ���̵忡 ���̴� ���� �ؽ��� ��ü�� �ʱ�ȭ�Ѵ�.
	if (!m_FadeRenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR))
	{
		return false;
	}


	// ���� ortho ������ ��ü�� ����ϴ�.
	m_SmallWindow = new OrthoWindowClass;
	if (!m_SmallWindow)
	{
		return false;
	}

	// ���� ortho ������ ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_SmallWindow->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight))
	{
		MessageBox(hwnd, L"Could not initialize the small ortho window object.", L"Error", MB_OK);
		return false;
	}




	// ��ü ȭ�� ortho window ��ü�� �����մϴ�.
	m_FullScreenWindow = new OrthoWindowClass;
	if (!m_FullScreenWindow)
	{
		return false;
	}

	// ��ü ȭ�� ortho window ��ü�� �ʱ�ȭ �մϴ�.
	if (!m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
	{
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}

	// ���̵� �� Ÿ���� 3000 �и� �ʷ� �����մϴ�.
	m_fadeInTime = 3000.0f;

	// ���� �� �ð��� 0 �и� �ʷ� �ʱ�ȭ�մϴ�.
	m_accumulatedTime = 0;

	// ���̵� ������� ó���� 0���� �ʱ�ȭ�Ͽ� ����� �˰� ǥ�õ˴ϴ�.
	m_fadePercentage = 0;

	// ȿ���� ��������� �����մϴ�.
	m_fadeDone = false;

	// ���̵� ���̴� ��ü�� ����ϴ�.
	m_FadeShader = new FadeShaderClass;
	if (!m_FadeShader)
	{
		return false;
	}

	// ���̵� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
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


	// ���� �ý��� ��ü ��ȯ
	if (m_SmallWindow)
	{
		m_SmallWindow->Shutdown();
		delete m_SmallWindow;
		m_SmallWindow = 0;
	}


	// ���� �ý��� ��ü ��ȯ
	if (m_FadeRenderTexture)
	{
		m_FadeRenderTexture->Shutdown();
		delete m_FadeRenderTexture;
		m_FadeRenderTexture = 0;
	}

	/*
	// m_Light ��ü ��ȯ
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



	// m_FireShader ��ü ��ȯ
	if (m_FireShader)
	{
		m_FireShader->Shutdown();
		delete m_FireShader;
		m_FireShader = 0;
	}

	// �� ��ü�� �����մϴ�.
	if (m_FireModel)
	{
		m_FireModel->Shutdown();
		delete m_FireModel;
		m_FireModel = 0;
	}

	// m_Text ��ü ��ȯ
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// m_Camera ��ü ��ȯ
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Direct3D ��ü ��ȯ
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
		// ���� �� �ð��� ������ ������ �ð� �߰��� ������Ʈ�Ͻʽÿ�.
		m_accumulatedTime += frameTime;

		// �ð��� ������ �� �������� ����ϴ� �ð���ŭ ���̵� ���� �����մϴ�.
		if (m_accumulatedTime < m_fadeInTime)
		{
			// ���� �� �ð��� �������� ȭ���� ������� ������ ����մϴ�.
			m_fadePercentage = m_accumulatedTime / m_fadeInTime;
		}
		else
		{
			// ���̵� �� Ÿ���� �Ϸ�Ǹ� ���̵� ȿ���� ���� ����� ���������� �������մϴ�.
			m_fadeDone = true;

			// ������� 100 %�� �����մϴ�.
			m_fadePercentage = 1.0f;
		}
	}


	// ī�޶��� ��ġ�� �����Ѵ�.
	m_Camera->SetRotation(rotation);


	/*
	// �ʴ� ������ ���� �����Ѵ�.
	if (!m_Text->SetFps(fps, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	// cpu ����� �����Ѵ�.
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

	// ��ü ����� ���� �ؽ�ó�� �������մϴ�.
	result = RenderSceneToTexture();
	if (!result)
	{
		return false;
	}

	// �������� ���� �ؽ�ó�� ���� ũ���� �ؽ��ķ� ���ø��Ѵ�.
	result = DownSampleTexture();
	if (!result)
	{
		return false;
	}

	// �ٿ� ���ø� �� ������ �ؽ�ó�� ���� ���� �����Ѵ�.
	result = RenderHorizontalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// ���� ���� ���� �ؽ�ó���� ���� ���� �����մϴ�.
	result = RenderVerticalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// ���� �帰 ������ �ؽ�ó�� ȭ�� ũ��� �ٽ� ���ø��Ѵ�.
	result = UpSampleTexture();
	if (!result)
	{
		return false;
	}

	// �帮�� ���ø� �� ������ �ؽ�ó�� ȭ�鿡 �������մϴ�.
	result = Render2DTextureScene();
	if (!result)
	{
		return false;
	}





	/*
	int renderCount;

	if (m_fadeDone)
	{
		// ���̵� ���� �Ϸ�Ǹ� �� ���۸� ����Ͽ� ����� ���������� �������մϴ�.
		RenderScene();
		m_Direct3D->EndScene();
	}
	else
	{
		// ���̵� ���� �Ϸ���� ���� ��� ����� �ؽ�ó�� �������ϰ� �ؽ�ó�� ���̵� ���մϴ�.
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

	// ������ ����� �������� �°� �����մϴ�.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� (ortho) ����� �����ɴϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// ������ ����
	XMMATRIX translateMatrix;
	XMFLOAT3 cameraPosition, modelPosition;

	// ī�޶� ��ġ�� ��´�.
	cameraPosition = m_Camera->GetPosition();

	// ������ ���� ��ġ�� �����Ѵ�.
	modelPosition.x = 0.0f;
	modelPosition.y = 1.0f;
	modelPosition.z = 3.0f;

	// ��ũ ź��Ʈ �Լ��� ����Ͽ� ���� ī�޶� ��ġ�� ���ϵ��� ������ �𵨿� �����ؾ��ϴ� ȸ���� ����մϴ�.
	double angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0 / XM_PI);

	// ȸ���� �������� ��ȯ�մϴ�.
	float rotation = (float)angle * 0.0174532925f;

	// ���� ����� �̿��Ͽ� �������� ������ ȸ���� �����մϴ�.
	worldMatrix = XMMatrixRotationY(rotation);

	// ������ ���� ��ġ�� �����Ѵ�.
	translateMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

	// ���������� rotation �� Translation ����� �����Ͽ� ������ ���� ���� ����� ����ϴ�.
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);




	// �� ���̴� ����
	static float frameTime = 0.0f;

	// ������ �ð� ī���͸� ������ŵ�ϴ�.
	frameTime += 0.01f;
	if (frameTime > 1000.0f)
	{
		frameTime = 0.0f;
	}

	// �� ���� �ٸ� ������ �ؽ�ó�� ���� �� ���� ��ũ�� �ӵ��� �����մϴ�.
	XMFLOAT3 scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f);

	// �� ���� ���� �ٸ� ������ ��Ÿ�� �ؽ�ó�� ����� �� ����� �� ���� �������� �����մϴ�.
	XMFLOAT3 scales = XMFLOAT3(1.0f, 2.0f, 3.0f);

	// �� ���� �ٸ� ������ �ؽ�ó�� ���� �� ���� �ٸ� x �� y �ְ� �μ��� �����մϴ�.
	XMFLOAT2 distortion1 = XMFLOAT2(0.1f, 0.2f);
	XMFLOAT2 distortion2 = XMFLOAT2(0.1f, 0.3f);
	XMFLOAT2 distortion3 = XMFLOAT2(0.1f, 0.1f);

	// �ؽ�ó ��ǥ ���ø� ������ �����ϰ� ���̾.
	float distortionScale = 0.8f;
	float distortionBias = 0.5f;






	// ȭ�� ������ ���� ���带 �մϴ�.
	m_Direct3D->TurnOnAlphaBlending();

	// ���簢�� ���� ������ �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
	m_FireModel->Render(m_Direct3D->GetDeviceContext());

	// ȭ�� ���̴��� ����Ͽ� �簢�� ���� �������մϴ�.
	if (!m_FireShader->Render(m_Direct3D->GetDeviceContext(), m_FireModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_FireModel->GetTexture(0), m_FireModel->GetTexture(1), m_FireModel->GetTexture(2),
		frameTime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale,
		distortionBias))
	{
		return false;
	}

	// ���� ������ ���ϴ�.
	m_Direct3D->TurnOffAlphaBlending();

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	// ����Ʈ�� �������� �ٽ� �����մϴ�.
	m_Direct3D->ResetViewport();

	return true;
}


bool GraphicsClass::DownSampleTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// ������ ����� �������� �°� �����մϴ�.
	m_DownSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_DownSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶�� d3d ��ü�κ��� ����� �� ��Ʈ������ ��´�.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ������ ũ�Ⱑ �۱� ������ ���������� �ؽ�ó�� ortho ����� �����ɴϴ�.
	m_DownSampleTexure->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// ������� �غ��ϱ� ���� �׷��� ������ ���ο� ���� ortho window ���ؽ��� �ε��� ���۸� �����ϴ�.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// �ؽ�ó ���̴��� ����Ͽ� ���� ortho â�� �������ϰ� ���� �ؽ�ó�� �ؽ�ó ���ҽ��� �������մϴ�.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix,
		orthoMatrix, m_RenderTexture->GetShaderResourceView()))
	{
		return false;
	}

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOn();

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	// ����Ʈ�� �������� �ٽ� �����մϴ�.
	m_Direct3D->ResetViewport();

	return true;
}




bool GraphicsClass::RenderHorizontalBlurToTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// ���� �� ���̴����� ���� float�� ȭ�� ���� �����մϴ�.
	float screenSizeX = (float)m_HorizontalBlurTexture->GetTextureWidth();

	// ������ ����� �������� �°� �����մϴ�.
	m_HorizontalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_HorizontalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶�� d3d ��ü�κ��� ����� �� ��Ʈ������ ��´�.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// �ؽ��İ� �ٸ� ������ �����Ƿ� ���������� ���� (ortho) ����� �ؽ�ó�� �����ɴϴ�.
	m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// ������� �غ��ϱ� ���� �׷��� ������ ���ο� ���� ortho window ���ؽ��� �ε��� ���۸� �����ϴ�.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// horizontal blur shader�� down sampled render�� ����Ͽ� ���� ortho �����츦 �ؽ�ó ���ҽ��� �������մϴ�.    
	if (!m_HorizontalblurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix,
		baseViewMatrix, orthoMatrix, m_DownSampleTexure->GetShaderResourceView(), screenSizeX))
	{
		return false;
	}

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOn();

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	// ����Ʈ�� �������� �ٽ� �����մϴ�.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderVerticalBlurToTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// ���� �� ���̴����� ���Ǵ� �ε� �Ҽ����� ȭ�� ���̸� �����մϴ�.
	float screenSizeY = (float)m_VerticalBlurTexture->GetTextureHeight();

	// ������ ����� �������� �°� �����մϴ�.
	m_VerticalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_VerticalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶�� d3d ��ü�κ��� ����� �� ��Ʈ������ ��´�.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// �ؽ��İ� �ٸ� ������ �����Ƿ� ���������� ���� (ortho) ����� �ؽ�ó�� �����ɴϴ�.
	m_VerticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// ������� �غ��ϱ� ���� �׷��� ������ ���ο� ���� ortho window ���ؽ��� �ε��� ���۸� �����ϴ�.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// ���� �� ���̴��� ���� �� ���� ����Ͽ� ���� ortho �����츦 �ؽ�ó ���ҽ��� �����մϴ�.
	if (!m_VerticalblurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix,
		baseViewMatrix, orthoMatrix, m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY))
	{
		return false;
	}

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOn();

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	// ����Ʈ�� �������� �ٽ� �����մϴ�.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::UpSampleTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// ������ ����� �������� �°� �����մϴ�.
	m_UpSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_UpSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶�� d3d ��ü�κ��� ����� �� ��Ʈ������ ��´�.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// �ؽ��İ� �ٸ� ������ �����Ƿ� ���������� ���� (ortho) ����� �ؽ�ó�� �����ɴϴ�.
	m_UpSampleTexure->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// �׷��� ������ ���ο� ��ü ȭ�� ���� ������ ���ؽ��� �ε��� ���۸� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// �ؽ�ó ���̴��� �ؽ�ó ���ҽ��� ���� ���� ũ���� ���� �帮�� �������� ����Ͽ� ��ü ȭ�� ortho â�� �������մϴ�.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix,
		baseViewMatrix, orthoMatrix, m_VerticalBlurTexture->GetShaderResourceView()))
	{
		return false;
	}

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOn();

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	// ����Ʈ�� �������� �ٽ� �����մϴ�.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::Render2DTextureScene()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	// ����� ������ ���۸� �����.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 0.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� (ortho) ����� �����ɴϴ�.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// �׷��� ������ ���ο� ��ü ȭ�� ���� ������ ���ؽ��� �ε��� ���۸� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// �ؽ�ó ���̴��� ����Ͽ� ��ü ȭ�� ortho â�� �������ϰ� �ؽ�ó ���ҽ��� ��ü ȭ�� ũ��� �帮�� �������մϴ�.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix,
		baseViewMatrix, orthoMatrix, m_UpSampleTexure->GetShaderResourceView()))
	{
		return false;
	}

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOn();

	// ������ �� ����� ȭ�鿡 ǥ���մϴ�.
	m_Direct3D->EndScene();

	return true;
}














/*
int GraphicsClass::RenderScene()
{

	// ����� ������ ���۸� �����.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� (ortho) ����� �����ɴϴ�.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	
	// ������ ����
	XMMATRIX translateMatrix;
	XMFLOAT3 cameraPosition, modelPosition;

	// ī�޶� ��ġ�� ��´�.
	cameraPosition = m_Camera->GetPosition();

	// ������ ���� ��ġ�� �����Ѵ�.
	modelPosition.x = 0.0f;
	modelPosition.y = 1.0f;
	modelPosition.z = 3.0f;

	// ��ũ ź��Ʈ �Լ��� ����Ͽ� ���� ī�޶� ��ġ�� ���ϵ��� ������ �𵨿� �����ؾ��ϴ� ȸ���� ����մϴ�.
	double angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0 / XM_PI);

	// ȸ���� �������� ��ȯ�մϴ�.
	float rotation = (float)angle * 0.0174532925f;

	// ���� ����� �̿��Ͽ� �������� ������ ȸ���� �����մϴ�.
	worldMatrix = XMMatrixRotationY(rotation);

	// ������ ���� ��ġ�� �����Ѵ�.
	translateMatrix = XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

	// ���������� rotation �� Translation ����� �����Ͽ� ������ ���� ���� ����� ����ϴ�.
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);




	// �� ���̴� ����
	static float frameTime = 0.0f;

	// ������ �ð� ī���͸� ������ŵ�ϴ�.
	frameTime += 0.01f;
	if (frameTime > 1000.0f)
	{
		frameTime = 0.0f;
	}

	// �� ���� �ٸ� ������ �ؽ�ó�� ���� �� ���� ��ũ�� �ӵ��� �����մϴ�.
	XMFLOAT3 scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f);

	// �� ���� ���� �ٸ� ������ ��Ÿ�� �ؽ�ó�� ����� �� ����� �� ���� �������� �����մϴ�.
	XMFLOAT3 scales = XMFLOAT3(1.0f, 2.0f, 3.0f);

	// �� ���� �ٸ� ������ �ؽ�ó�� ���� �� ���� �ٸ� x �� y �ְ� �μ��� �����մϴ�.
	XMFLOAT2 distortion1 = XMFLOAT2(0.1f, 0.2f);
	XMFLOAT2 distortion2 = XMFLOAT2(0.1f, 0.3f);
	XMFLOAT2 distortion3 = XMFLOAT2(0.1f, 0.1f);

	// �ؽ�ó ��ǥ ���ø� ������ �����ϰ� ���̾.
	float distortionScale = 0.8f;
	float distortionBias = 0.5f;






	// ȭ�� ������ ���� ���带 �մϴ�.
	m_Direct3D->TurnOnAlphaBlending();

	// ���簢�� ���� ������ �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
	m_FireModel->Render(m_Direct3D->GetDeviceContext());

	// ȭ�� ���̴��� ����Ͽ� �簢�� ���� �������մϴ�.
	if (!m_FireShader->Render(m_Direct3D->GetDeviceContext(), m_FireModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_FireModel->GetTexture(0), m_FireModel->GetTexture(1), m_FireModel->GetTexture(2),
		frameTime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale,
		distortionBias))
	{
		return false;
	}

	// ���� ������ ���ϴ�.
	m_Direct3D->TurnOffAlphaBlending();

	return 1;
}


int GraphicsClass::RenderToFadeTexture()
{
	// ������ ����� �������� �°� �����մϴ�.
	m_FadeRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� �� ��ϴ�.
	m_FadeRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	int renderCount = RenderScene();

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	return renderCount;
}


bool GraphicsClass::RenderFadingScene()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	bool result;

	// ����� ������ ���۸� �����.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� (ortho) ����� �����ɴϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// ��Ʈ �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// ���̵� ���̴��� ����Ͽ� ��Ʈ ���� �������մϴ�.
	result = m_FadeShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_FadeRenderTexture->GetShaderResourceView(), m_fadePercentage);
	if (!result)
	{
		return false;
	}

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOn();

	// ������ �� ����� ȭ�鿡 ǥ���մϴ�.
	m_Direct3D->EndScene();

	return true;
}
*/

bool GraphicsClass::RenderText(int renderCount)
{
	XMMATRIX worldMatrix, orthoMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// �� �����ӿ��� ������ ������ �� ���� ���� �����Ѵ�.
	if (!m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	// �ؽ�Ʈ�� �������ϱ� ���� ���� ������ �Ҵ�.
	m_Direct3D->TurnOnAlphaBlending();

	// �ؽ�Ʈ ���ڿ��� ������ �Ѵ�.
	if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
	{
		return false;
	}

	// �ؽ�Ʈ�� ������ �� �� ���� ������ �����Ѵ�.
	m_Direct3D->TurnOffAlphaBlending();

	m_Direct3D->EndScene();

	return true;
}
