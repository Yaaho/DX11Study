#include "Stdafx.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "TextClass.h"
#include "ModelClass.h"

#include "LightClass.h"
#include "DepthShaderClass.h"
#include "ShadowShaderClass.h"
#include "TextureShaderClass.h"

#include "HorizontalBlurShaderClass.h"
#include "VerticalBlurShaderClass.h"
#include "SoftShadowShaderClass.h"

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
	m_Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -10.0f));
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

	if (!m_CubeModel->Initialize(m_Direct3D->GetDevice(), "data/cube.txt") || !m_CubeModel->LoadTextures(m_Direct3D->GetDevice(), L"data/wall01.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
		return false;
	}

	// ť�� ���� ��ġ�� ??���� �մϴ�.
	m_CubeModel->SetPosition(-2.0f, 2.0f, 0.0f);


	// ���� �� ��ü�� ����ϴ�.
	m_GroundModel = new ModelClass;
	if (!m_GroundModel)
	{
		return false;
	}

	// ���� �� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_GroundModel->Initialize(m_Direct3D->GetDevice(), "data/plane01.txt") || !m_GroundModel->LoadTextures(m_Direct3D->GetDevice(), L"data/metal001.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}

	// ���� ���� ��ġ�� ??�����մϴ�.
	m_GroundModel->SetPosition(0.0f, 1.0f, 0.0f);


	// ���� �� ��ü�� ����ϴ�.
	m_SphereModel = new ModelClass;
	if (!m_SphereModel)
	{
		return false;
	}

	// ���� �� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_SphereModel->Initialize(m_Direct3D->GetDevice(), "data/sphere.txt") || !m_SphereModel->LoadTextures(m_Direct3D->GetDevice(), L"data/ice.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK);
		return false;
	}

	// ���� ���� ��ġ�� ??�����մϴ�.
	m_SphereModel->SetPosition(2.0f, 2.0f, 0.0f);



	// light ��ü�� ����ϴ�.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// ���� ��ü�� �ʱ�ȭ�մϴ�.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);



	// ���� ���̴� ��ü�� ����ϴ�.
	m_DepthShader = new DepthShaderClass;
	if (!m_DepthShader)
	{
		return false;
	}

	// ���� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return false;
	}


	// �׸��� ���̴� ��ü�� ����ϴ�.
	m_ShadowShader = new ShadowShaderClass;
	if (!m_ShadowShader)
	{
		return false;
	}

	// �׸��� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_ShadowShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
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
	m_HorizontalBlurShader = new HorizontalBlurShaderClass;
	if (!m_HorizontalBlurShader)
	{
		return false;
	}

	// ���� �� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_HorizontalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur shader object.", L"Error", MB_OK);
		return false;
	}


	// ���� �� ���̴� ������Ʈ�� �����Ѵ�.
	m_VerticalBlurShader = new VerticalBlurShaderClass;
	if (!m_VerticalBlurShader)
	{
		return false;
	}

	// ���� �� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_VerticalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur shader object.", L"Error", MB_OK);
		return false;
	}


	// �ε巯�� �׸��� ���̴� ��ü�� ����ϴ�.
	m_SoftShadowShader = new SoftShadowShaderClass;
	if (!m_SoftShadowShader)
	{
		return false;
	}

	// �ε巯�� �׸��� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_SoftShadowShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the soft shadow shader object.", L"Error", MB_OK);
		return false;
	}



	// �������� �ؽ�ó ������Ʈ�� �����Ѵ�.
	m_DepthMapTexture = new RenderTextureClass;
	if (!m_DepthMapTexture)
	{
		return false;
	}

	// �������� �ؽ�ó ������Ʈ�� �ʱ�ȭ�Ѵ�.
	if (!m_DepthMapTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT,
		SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}


	// ��� �������� �ؽ�ó ������Ʈ�� �����Ѵ�.
	m_BlackWhiteRenderTexture = new RenderTextureClass;
	if (!m_BlackWhiteRenderTexture)
	{
		return false;
	}

	// ��� �������� �ؽ�ó ������Ʈ�� �ʱ�ȭ�Ѵ�.
	if (!m_BlackWhiteRenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH,
		SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize the black and white render to texture object.", L"Error", MB_OK);
		return false;
	}

	// ũ�⸦ ���÷� �����մϴ�.
	int downSampleWidth = SHADOWMAP_WIDTH / 2;
	int downSampleHeight = SHADOWMAP_HEIGHT / 2;

	// �ٿ� ���� �������� �ؽ�ó ������Ʈ�� �����Ѵ�.
	m_DownSampleTexure = new RenderTextureClass;
	if (!m_DownSampleTexure)
	{
		return false;
	}

	// �ٿ� ���� ������ �ؽ�ó ������Ʈ�� �ʱ�ȭ�Ѵ�.
	if (!m_DownSampleTexure->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, 100.0f, 1.0f))
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
		0.1f))
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
		0.1f))
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur render to texture object.", L"Error", MB_OK);
		return false;
	}




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

	if (m_SmallWindow)
	{
		m_SmallWindow->Shutdown();
		delete m_SmallWindow;
		m_SmallWindow = 0;
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


	// ���� �� �ؽ��� ��ü�� �����մϴ�.
	if (m_VerticalBlurTexture)
	{
		m_VerticalBlurTexture->Shutdown();
		delete m_VerticalBlurTexture;
		m_VerticalBlurTexture = 0;
	}

	// ���� �� �ؽ��� ��ü�� �����մϴ�.
	if (m_HorizontalBlurTexture)
	{
		m_HorizontalBlurTexture->Shutdown();
		delete m_HorizontalBlurTexture;
		m_HorizontalBlurTexture = 0;
	}

	// ���� �� �ؽ��� ��ü�� �����մϴ�.
	if (m_DownSampleTexure)
	{
		m_DownSampleTexure->Shutdown();
		delete m_DownSampleTexure;
		m_DownSampleTexure = 0;
	}

	// ���� �� �ؽ��� ��ü�� �����մϴ�.
	if (m_BlackWhiteRenderTexture)
	{
		m_BlackWhiteRenderTexture->Shutdown();
		delete m_BlackWhiteRenderTexture;
		m_BlackWhiteRenderTexture = 0;
	}



	// ���� �� �ؽ��� ��ü�� �����մϴ�.
	if (m_DepthMapTexture)
	{
		m_DepthMapTexture->Shutdown();
		delete m_DepthMapTexture;
		m_DepthMapTexture = 0;
	}

	// �ε巯�� �׸��� ���̴� ��ü�� �����Ѵ�.
	if (m_SoftShadowShader)
	{
		m_SoftShadowShader->Shutdown();
		delete m_SoftShadowShader;
		m_SoftShadowShader = 0;
	}

	// ���� �� ���̴� ��ü�� �����Ѵ�.
	if (m_VerticalBlurShader)
	{
		m_VerticalBlurShader->Shutdown();
		delete m_VerticalBlurShader;
		m_VerticalBlurShader = 0;
	}

	// ���� �� ���̴� ��ü�� �����մϴ�.
	if (m_HorizontalBlurShader)
	{
		m_HorizontalBlurShader->Shutdown();
		delete m_HorizontalBlurShader;
		m_HorizontalBlurShader = 0;
	}





	// �׸��� ���̴� ��ü�� �����մϴ�.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// �׸��� ���̴� ��ü�� �����մϴ�.
	if (m_ShadowShader)
	{
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	}


	// ���� ���̴� ��ü�� �����մϴ�.
	if (m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	// ���� ��ü�� �����մϴ�.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// ���� �� ��ü�� �����մϴ�.
	if (m_SphereModel)
	{
		m_SphereModel->Shutdown();
		delete m_SphereModel;
		m_SphereModel = 0;
	}


	// ���� �� ��ü�� �����մϴ�.
	if (m_GroundModel)
	{
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
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


	static float lightPositionX = -5.0f;

	// �� �������� ���� ��ġ�� ������Ʈ�մϴ�.
	lightPositionX += 0.05f;
	if (lightPositionX > 5.0f)
	{
		lightPositionX = -5.0f;
	}

	// ���� ��ġ�� ������Ʈ�մϴ�.
	m_Light->SetPosition(lightPositionX, 8.0f, -5.0f);


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

	RenderScene();
	m_Direct3D->EndScene();

	/*
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
	*/


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
	bool result;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	float posX, posY, posZ;


	// ���� ����� �ؽ�ó�� �������մϴ�. 
	result = RenderToDepthMapTexture();
	if (!result)
	{
		return false;
	}

	// �������� �׸��ڰ��ִ� ����� ������� �������մϴ�. 
	result = RenderBlackAndWhiteShadows();
	if (!result)
	{
		return false;
	}

	// �׷� ���� ��� �� �ؽ�ó�� ���ø��մϴ�. 
	result = DownSampleTexture();
	if (!result)
	{
		return false;
	}

	// �ٿ� ���ø� �� �ؽ�ó�� ���� ���� �����մϴ�. 
	result = RenderHorizontalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// ���� �ؽ�ó�� ���� ���� �����մϴ�. 
	result = RenderVerticalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// ���۸� ����� ����� �����մϴ�. 
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ������ ��ġ�� ���� ������ ����� �����մϴ�.
	m_Light->GenerateViewMatrix();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// ť�� �𵨿� ���� ��ȯ ����� �����Ͻʽÿ�. 
	m_CubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// �ε巯�� �׸��� ���̴��� ����Ͽ� ť�� ���� �������մϴ�. 
	m_CubeModel->Render(m_Direct3D->GetDeviceContext());
	result = m_SoftShadowShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix,
		viewMatrix, projectionMatrix, m_CubeModel->GetTexture(0),
		m_VerticalBlurTexture->GetShaderResourceView(), m_Light->GetPosition(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// ���� ����� �缳���մϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ���� �𵨿� ���� ��ȯ ����� �����մϴ�.
	m_SphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// �ε巯�� �׸��� ���̴��� ����Ͽ� ���� ���� �������մϴ�. 
	m_SphereModel->Render(m_Direct3D->GetDeviceContext());
	result = m_SoftShadowShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix,
		viewMatrix, projectionMatrix, m_SphereModel->GetTexture(0),
		m_VerticalBlurTexture->GetShaderResourceView(), m_Light->GetPosition(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}

	// ���� ����� �缳���մϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground �𵨿� ���� ��ȯ ����� �����մϴ�.
	m_GroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// �ε巯�� �׸��� ���̴��� ����Ͽ� �׶��� ���� �������մϴ�. 
	m_GroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_SoftShadowShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix,
		viewMatrix, projectionMatrix, m_GroundModel->GetTexture(0),
		m_VerticalBlurTexture->GetShaderResourceView(), m_Light->GetPosition(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}


	return true;
}


bool GraphicsClass::RenderToDepthMapTexture()
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix;
	float posX, posY, posZ;
	bool result;

	// ������ ����� �������� �°� �����մϴ�.
	m_DepthMapTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_DepthMapTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ������ ��ġ�� ���� ������ ����� �����մϴ�.
	m_Light->GenerateViewMatrix();

	// d3d ��ü���� ���� ����� �����ɴϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ����Ʈ ������Ʈ���� �� �� ���簢�� ��Ʈ������ �����ɴϴ�.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// ť�� �𵨿� ���� ��ȯ ����� �����Ͻʽÿ�.
	m_CubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// ���� ���̴��� ť�� ���� �������մϴ�.
	m_CubeModel->Render(m_Direct3D->GetDeviceContext());
	result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix,
		lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// ���� ����� �缳���մϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ���� �𵨿� ���� ��ȯ ����� �����մϴ�.
	m_SphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// ���� ���̴��� ���� ���� �������մϴ�.
	m_SphereModel->Render(m_Direct3D->GetDeviceContext());
	result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix,
		lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// ���� ����� �缳���մϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground �𵨿� ���� ��ȯ ����� �����մϴ�.
	m_GroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// ���� ���̴��� �׶��� ���� �������մϴ�.
	m_GroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix,
		lightViewMatrix, lightProjectionMatrix);
	if (!result)
	{
		return false;
	}

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	// ����Ʈ�� �������� �ٽ� �����մϴ�.
	m_Direct3D->ResetViewport();

	return true;
}



bool GraphicsClass::RenderBlackAndWhiteShadows()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	float posX, posY, posZ;
	bool result;


	// ������ ����� �������� �°� �����մϴ�.
	m_BlackWhiteRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_BlackWhiteRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ������ ��ġ�� ���� ������ ����� �����մϴ�.
	m_Light->GenerateViewMatrix();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�. 
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// ����Ʈ ������Ʈ�κ��� ����Ʈ�� ��� ���� ����� �����ɴϴ�. 
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// ť�� �𵨿� ���� ��ȯ ����� �����Ͻʽÿ�. 
	m_CubeModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// �׸��� ���̴��� ����Ͽ� ť�� ���� �������մϴ�. 
	m_CubeModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix,
		viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix,
		m_DepthMapTexture->GetShaderResourceView(), m_Light->GetPosition());
	if (!result)
	{
		return false;
	}

	// ���� ����� �缳���մϴ�. 
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ���� �𵨿� ���� ��ȯ ����� �����մϴ�. 
	m_SphereModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// �׸��� ���̴��� ����Ͽ� ���� ���� �������մϴ�. 
	m_SphereModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix,
		viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix,
		m_DepthMapTexture->GetShaderResourceView(), m_Light->GetPosition());
	if (!result)
	{
		return false;
	}

	// ���� ����� �缳���մϴ�. 
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ground �𵨿� ���� ��ȯ ����� �����մϴ�. 
	m_GroundModel->GetPosition(posX, posY, posZ);
	worldMatrix = XMMatrixTranslation(posX, posY, posZ);

	// �׸��� ���̴��� ����Ͽ� �׶��� ���� �������մϴ�. 
	m_GroundModel->Render(m_Direct3D->GetDeviceContext());
	result = m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix,
		viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix,
		m_DepthMapTexture->GetShaderResourceView(), m_Light->GetPosition());
	if (!result)
	{
		return false;
	}

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� ������ ����� �� �̻� �ؽ�ó�� �ٽ� �������� �ʽ��ϴ�. 
	m_Direct3D->SetBackBufferRenderTarget();

	// ����Ʈ�� �������� �ٽ� �����մϴ�. 
	m_Direct3D->ResetViewport();

	return true;
}



bool GraphicsClass::DownSampleTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	bool result;


	// ������ ����� �������� �°� �����մϴ�. 
	m_DownSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�. 
	m_DownSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�. 
	m_Camera->Render();

	// ī�޶�� d3d ��ü�κ��� ����� �� ��Ʈ������ ��´�. 
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);

	// ������ ũ�Ⱑ �۱� ������ ���������� �ؽ�ó�� ortho ����� �����ɴϴ�. 
	m_DownSampleTexure->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�. 
	m_Direct3D->TurnZBufferOff();

	// ������� �غ��ϱ� ���� �׷��� ������ ���ο� ���� ortho window ���ؽ��� �ε��� ���۸� �����ϴ�. 
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// �ؽ�ó ���̴��� ����Ͽ� ���� ortho â�� �������ϰ� ���� �ؽ�ó�� �ؽ�ó ���ҽ��� �������մϴ�. 
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix,
		baseViewMatrix, orthoMatrix, m_BlackWhiteRenderTexture->GetShaderResourceView());
	if (!result)
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
	float screenSizeX;
	bool result;


	// ���� �� ���̴����� ���� float�� ȭ�� ���� �����մϴ�. 
	screenSizeX = (float)(SHADOWMAP_WIDTH / 2);

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
	result = m_HorizontalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix,
		baseViewMatrix, orthoMatrix, m_DownSampleTexure->GetShaderResourceView(),
		screenSizeX);
	if (!result)
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
	float screenSizeY;
	bool result;


	// ���� �� ���̴����� ���Ǵ� �ε� �Ҽ����� ȭ�� ���̸� �����մϴ�. 
	screenSizeY = (float)(SHADOWMAP_HEIGHT / 2);

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
	result = m_VerticalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix,
		baseViewMatrix, orthoMatrix, m_HorizontalBlurTexture->GetShaderResourceView(),
		screenSizeY);
	if (!result)
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
