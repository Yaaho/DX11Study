#include "Stdafx.h"
#include "D3DClass.h"
#include "ShaderManagerClass.h"
#include "CameraClass.h"
#include "TextClass.h"
#include "ModelClass.h"
#include "BumpModelClass.h"

#include "LightClass.h"


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

	// ���̴� ������ ��ü�� ����ϴ�.
	m_ShaderManager = new ShaderManagerClass;
	if (!m_ShaderManager)
	{
		return false;
	}

	// ���̴� ������ ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the shader manager object.", L"Error", MB_OK);
		return false;
	}

	// m_Camera ��ü ����
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// ī�޶��� �ʱ� ��ġ�� ȸ���� �����մϴ�.
	m_Camera->SetPosition(XMFLOAT3(0.0f, 7.0f, -10.0f));
	m_Camera->SetRotation(XMFLOAT3(35.0f, 0.0f, 0.0f));
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



	// ť�� �� ������Ʈ�� �����մϴ�.
	m_CubeModel = new ModelClass;
	if (!m_CubeModel)
	{
		return false;
	}

	if (!m_CubeModel->Initialize(m_Direct3D->GetDevice(), "data/cube.txt") || !m_CubeModel->LoadTextures(m_Direct3D->GetDevice(), L"data/seafloor.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
		return false;
	}


	// ť�� �� ������Ʈ�� �����մϴ�.
	m_Model2 = new ModelClass;
	if (!m_Model2)
	{
		return false;
	}

	if (!m_Model2->Initialize(m_Direct3D->GetDevice(), "data/cube.txt") || !m_Model2->LoadTextures(m_Direct3D->GetDevice(), L"data/metal.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
		return false;
	}


	// ���� �ʰ� ���� ���Ͱ��ִ� �𵨿� ���� �� ��° ���� �� ��ü�� ����ϴ�.
	m_Model3 = new BumpModelClass;
	if (!m_Model3)
	{
		return false;
	}

	// ���� �� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_Model3->Initialize(m_Direct3D->GetDevice(), "data/cube.txt", L"data/stone.dds",
		L"data/normal.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the third model object.", L"Error", MB_OK);
		return false;
	}



	// ���� ��ü�� �����մϴ�.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// ���� ��ü�� �ʱ�ȭ�մϴ�.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(64.0f);



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
	if (m_FadeRenderTexture)
	{
		m_FadeRenderTexture->Shutdown();
		delete m_FadeRenderTexture;
		m_FadeRenderTexture = 0;
	}


	// ���� ��ü�� �����մϴ�.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}


	if (m_Model3)
	{
		m_Model3->Shutdown();
		delete m_Model3;
		m_Model3 = 0;
	}


	if (m_Model2)
	{
		m_Model2->Shutdown();
		delete m_Model2;
		m_Model2 = 0;
	}

	// ť�� �� ��ü�� �����մϴ�.
	if (m_CubeModel)
	{
		m_CubeModel->Shutdown();
		delete m_CubeModel;
		m_CubeModel = 0;
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

bool GraphicsClass::Frame(float frameTime, float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
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


	// ī�޶� ��ġ�� �����մϴ�.
	m_Camera->SetPosition(XMFLOAT3(posX, posY, posZ));
	m_Camera->SetRotation(XMFLOAT3(rotX, rotY, rotZ));




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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;

	// ���� �׸��� ���� ���۸� ����ϴ�
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// ù ��° ���� ȸ�� �� ���� �̵��� �����մϴ�.
	translateMatrix = XMMatrixTranslation(-3.5f, 0.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

	// �ؽ�ó ���̴��� ����Ͽ� ù ��° ���� �������մϴ�.
	m_CubeModel->Render(m_Direct3D->GetDeviceContext());

	if (!m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix,
		m_CubeModel->GetTexture(0)))
	{
		return false;
	}

	m_Direct3D->GetWorldMatrix(worldMatrix);
	translateMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

	// ����Ʈ ���̴��� ����Ͽ� �� ��° ���� �������մϴ�.
	m_Model2->Render(m_Direct3D->GetDeviceContext());

	if (!m_ShaderManager->RenderLightShader(m_Direct3D->GetDeviceContext(), m_Model2->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model2->GetTexture(0), m_Light->GetDirection(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(),
		m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))
	{
		return false;
	}

	m_Direct3D->GetWorldMatrix(worldMatrix);
	translateMatrix = XMMatrixTranslation(3.5f, 0.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

	// ���� �� ���̴��� ����Ͽ� �� ��° ���� �������մϴ�.
	m_Model3->Render(m_Direct3D->GetDeviceContext());

	if (!m_ShaderManager->RenderBumpMapShader(m_Direct3D->GetDeviceContext(), m_Model3->GetIndexCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model3->GetColorTexture(), m_Model3->GetNormalMapTexture(),
		m_Light->GetDirection(),
		m_Light->GetDiffuseColor()))
	{
		return false;
	}

	return true;
}




bool GraphicsClass::RenderToFadeTexture()
{
	// ������ ����� �������� �°� �����մϴ�.
	m_FadeRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� �� ��ϴ�.
	m_FadeRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	bool Result = RenderScene();

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	return Result;
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
	m_Direct3D->EnableAlphaBlending();

	// �ؽ�Ʈ ���ڿ��� ������ �Ѵ�.
	if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
	{
		return false;
	}

	// �ؽ�Ʈ�� ������ �� �� ���� ������ �����Ѵ�.
	m_Direct3D->DisableAlphaBlending();

	m_Direct3D->EndScene();

	return true;
}
