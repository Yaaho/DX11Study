#include "Stdafx.h"
#include "D3DClass.h"
#include "ShaderManagerClass.h"
#include "CameraClass.h"
#include "TextClass.h"

#include "TextureShaderClass.h"
#include "BitmapClass.h"
#include "HorizontalBlurShaderClass.h"
#include "VerticalBlurShaderClass.h"
#include "GlowMapShaderClass.h"
#include "GlowShaderClass.h"



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
	m_Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -5.0f));
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


	/*
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
	*/

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


	// ��Ʈ �� ��ü�� ����ϴ�.
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
	{
		return false;
	}

	/*
	// ��Ʈ �� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_Bitmap->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, L"data/test.dds",
		L"data/glowmap.dds", 256, 32))
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}
	*/

	// ���� ���� ���̴� ��ü�� ����ϴ�.
	m_HorizontalBlurShader = new HorizontalBlurShaderClass;
	if (!m_HorizontalBlurShader)
	{
		return false;
	}

	// ���� ���� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_HorizontalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur shader object.", L"Error", MB_OK);
		return false;
	}

	// ���� ���� ���̴� ������Ʈ�� �����Ѵ�.
	m_VerticalBlurShader = new VerticalBlurShaderClass;
	if (!m_VerticalBlurShader)
	{
		return false;
	}

	// ���� ���� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_VerticalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur shader object.", L"Error", MB_OK);
		return false;
	}


	// �۷ο� �� ���̴� ��ü�� ����ϴ�.
	m_GlowMapShader = new GlowMapShaderClass;
	if (!m_GlowMapShader)
	{
		return false;
	}

	// �۷ο� �� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_GlowMapShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the glow map shader object.", L"Error", MB_OK);
		return false;
	}

	// �۷ο� ���̴� ��ü�� ����ϴ�.
	m_GlowShader = new GlowShaderClass;
	if (!m_GlowShader)
	{
		return false;
	}

	// �۷ο� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_GlowShader->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the glow shader object.", L"Error", MB_OK);
		return false;
	}


	// ������ �ؽ�ó ��ü�� �����Ѵ�.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// ������ �ؽ�ó ��ü�� �ʱ�ȭ�Ѵ�.
	if (!m_RenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
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
	if (!m_DownSampleTexure->Initialize(m_Direct3D->GetDevice(), (screenWidth / 2), (screenHeight / 2)))
	{
		MessageBox(hwnd, L"Could not initialize the down sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// �ؽ�ó ��ü�� ���� ���� �������� ����ϴ�.
	m_HorizontalBlurTexture = new RenderTextureClass;
	if (!m_HorizontalBlurTexture)
	{
		return false;
	}

	// �ؽ�ó ��ü�� ���� ���� �������� �ʱ�ȭ�մϴ�.
	if (!m_HorizontalBlurTexture->Initialize(m_Direct3D->GetDevice(), (screenWidth / 2), (screenHeight / 2)))
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// �ؽ�ó ������Ʈ�� ���� ���� ������ ����ϴ�.
	m_VerticalBlurTexture = new RenderTextureClass;
	if (!m_VerticalBlurTexture)
	{
		return false;
	}

	// �ؽ�ó ������Ʈ�� ���� ���� ������ �ʱ�ȭ�մϴ�.
	if (!m_VerticalBlurTexture->Initialize(m_Direct3D->GetDevice(), (screenWidth / 2), (screenHeight / 2)))
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
	if (!m_FadeRenderTexture->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight))
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
	if (!m_SmallWindow->Initialize(m_Direct3D->GetDevice(), (screenWidth / 2), (screenHeight / 2)))
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

	// ���� ortho ������ ��ü�� �����ϴ�.
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


	// ���� ���� ������ �ؽ�ó ��ü�� �����ϴ�.
	if (m_VerticalBlurTexture)
	{
		m_VerticalBlurTexture->Shutdown();
		delete m_VerticalBlurTexture;
		m_VerticalBlurTexture = 0;
	}

	// ���� ���� ������ �ؽ�ó ��ü�� �����ϴ�.
	if (m_HorizontalBlurTexture)
	{
		m_HorizontalBlurTexture->Shutdown();
		delete m_HorizontalBlurTexture;
		m_HorizontalBlurTexture = 0;
	}



	// �ٿ� ���� ������ �ؽ��� ��ü�� �������Ѵ�.
	if (m_DownSampleTexure)
	{
		m_DownSampleTexure->Shutdown();
		delete m_DownSampleTexure;
		m_DownSampleTexure = 0;
	}



	// ������ �ؽ��� ��ü�� �������Ѵ�.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}



	// �۷ο� ���̴� ��ü�� �����մϴ�.
	if (m_GlowShader)
	{
		m_GlowShader->Shutdown();
		delete m_GlowShader;
		m_GlowShader = 0;
	}

	// �۷ο� �� ���̴� ��ü�� �����մϴ�.
	if (m_GlowMapShader)
	{
		m_GlowMapShader->Shutdown();
		delete m_GlowMapShader;
		m_GlowMapShader = 0;
	}


	// ���� ���� ���̴� ��ü�� �����Ѵ�.
	if (m_VerticalBlurShader)
	{
		m_VerticalBlurShader->Shutdown();
		delete m_VerticalBlurShader;
		m_VerticalBlurShader = 0;
	}


	// ���� ���� ���̴� ��ü�� �����մϴ�.
	if (m_HorizontalBlurShader)
	{
		m_HorizontalBlurShader->Shutdown();
		delete m_HorizontalBlurShader;
		m_HorizontalBlurShader = 0;
	}


	// ��Ʈ �� ��ü�� �����մϴ�.
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}


	// �ؽ�ó ���̴� ��ü�� �����Ѵ�.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
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

	RenderScene();


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
	// �۷ο� ���� ������ �ؽ�ó�� ���� �������մϴ�.
	bool result = RenderGlowMapToTexture();
	if (!result)
	{
		return false;
	}

	// �������� ���� �ؽ�ó�� ���� ũ���� �ؽ�ó�� ���ø��մϴ�.
	result = DownSampleTexture();
	if (!result)
	{
		return false;
	}

	// �ٿ� ���ø� �� ������ �ؽ�ó�� ���� ������ �����մϴ�.
	result = RenderHorizontalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// ���� ���� �帲 ���� �ؽ�ó���� ���� �帲 ȿ���� �����մϴ�.
	result = RenderVerticalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// �Ϲ� UI ��Ҹ� ��ü ȭ�� �ؽ�ó�� �������մϴ�.
	result = RenderUIElementsToTexture();
	if (!result)
	{
		return false;
	}

	// UI ��ҿ� ������ UI ��Ҹ� ������ ���� ����� �������մϴ�.
	RenderGlowScene();


	return true;
}



bool GraphicsClass::RenderGlowMapToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// ������ ����� �������� �°� �����մϴ�.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// ��Ʈ �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
	if (!m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 100, 100))
	{
		return false;
	}

	/*
	// �۷ο� �� ���̴��� ����Ͽ� ��Ʈ ���� �������մϴ�.
	m_GlowMapShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix,
		orthoMatrix, m_Bitmap->GetTexture(), m_Bitmap->GetGlowMap());
	*/


	//��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOn();

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	// ����Ʈ�� �������� �ٽ� �����մϴ�.
	m_Direct3D->ResetViewport();

	return true;
}


bool GraphicsClass::DownSampleTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// ������ ����� �������� �°� �����մϴ�.
	m_DownSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_DownSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶�� d3d ��ü�κ��� ����� �� ��Ʈ������ ��´�.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// ������ ũ�Ⱑ �۱� ������ ���������� �ؽ�ó�� ortho ����� �����ɴϴ�.
	// m_DownSampleTexure->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// ������� �غ��ϱ� ���� �׷��� ������ ���ο� ���� ortho window ���ؽ��� �ε��� ���۸� �����ϴ�.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	/*
	// �ؽ�ó ���̴��� ����Ͽ� ���� ortho â�� �������ϰ� ���� �ؽ�ó�� �ؽ�ó ���ҽ��� �������մϴ�.
	if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, viewMatrix,
		orthoMatrix, m_RenderTexture->GetShaderResourceView()))
	{
		return false;
	}
	*/

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
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// ������ ����� �������� �°� �����մϴ�.
	m_HorizontalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_HorizontalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶�� d3d ��ü�κ��� ����� �� ��Ʈ������ ��´�.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// �ؽ��İ� �ٸ� ������ �����Ƿ� ���������� ���� (ortho) ����� �ؽ�ó�� �����ɴϴ�.
	// m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// ���� ���� ���̴����� ���� float�� ȭ�� ���� �����մϴ�.
	float screenSizeX = (float)m_HorizontalBlurTexture->GetTextureWidth();

	// ������� �غ��ϱ� ���� �׷��� ������ ���ο� ���� ortho window ���ؽ��� �ε��� ���۸� �����ϴ�.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	/* 
	// horizontal blur shader�� down sampled render�� ����Ͽ� ���� ortho �����츦 �ؽ�ó ���ҽ��� �������մϴ�.    
	if (!m_HorizontalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix,
		viewMatrix, orthoMatrix, m_DownSampleTexure->GetShaderResourceView(), screenSizeX))
	{
		return false;
	}
	*/

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
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// ������ ����� �������� �°� �����մϴ�.
	m_VerticalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_VerticalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶�� d3d ��ü�κ��� ����� �� ��Ʈ������ ��´�.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// �ؽ��İ� �ٸ� ������ �����Ƿ� ���������� ���� (ortho) ����� �ؽ�ó�� �����ɴϴ�.
	// m_VerticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// ���� ���� ���̴����� ���Ǵ� �ε� �Ҽ����� ȭ�� ���̸� �����մϴ�.
	float screenSizeY = (float)m_VerticalBlurTexture->GetTextureHeight();

	// ������� �غ��ϱ� ���� �׷��� ������ ���ο� ���� ortho window ���ؽ��� �ε��� ���۸� �����ϴ�.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	/*
	// ���� ���� ���̴��� ���� ���� ���� ����Ͽ� ���� ortho �����츦 �ؽ�ó ���ҽ��� �����մϴ�.
	if (!m_VerticalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix,
		viewMatrix, orthoMatrix, m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY))
	{
		return false;
	}
	*/

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOn();

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	// ����Ʈ�� �������� �ٽ� �����մϴ�.
	m_Direct3D->ResetViewport();

	return true;
}


bool GraphicsClass::RenderUIElementsToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	bool result;


	// ������ ����� �������� �°� �����մϴ�.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// �������� �ؽ�ó�� ����ϴ�.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� (ortho) ����� �����ɴϴ�.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// ��Ʈ �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
	result = m_Bitmap->Render(m_Direct3D->GetDeviceContext(), 100, 100);
	if (!result)
	{
		return false;
	}

	// �ؽ�ó ���̴��� ����Ͽ� ��Ʈ ���� �������մϴ�.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix,
		orthoMatrix, m_Bitmap->GetTexture());
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


bool GraphicsClass::RenderGlowScene()
{
	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	// ����� ������ ���۸� �����.
	m_Direct3D->BeginScene(1.0f, 0.0f, 0.0f, 0.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� (ortho) ����� �����ɴϴ�.
	m_Camera->GetBaseViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	// ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
	m_Direct3D->TurnZBufferOff();

	// �׷��� ������ ���ο� ��ü ȭ�� ���� ������ ���ؽ��� �ε��� ���۸� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// �ؽ�ó ���̴��� ����Ͽ� ��ü ȭ�� ortho â�� �������ϰ� �ؽ�ó ���ҽ��� ��ü ȭ�� ũ��� �帮�� �������մϴ�.
	m_GlowShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, viewMatrix,
		orthoMatrix, m_RenderTexture->GetShaderResourceView(), m_VerticalBlurTexture->GetShaderResourceView(), 3.0f);

	// ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
	m_Direct3D->TurnZBufferOn();

	// ������ �� ����� ȭ�鿡 ǥ���մϴ�.
	m_Direct3D->EndScene();

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

	/*
	// �� �����ӿ��� ������ ������ �� ���� ���� �����Ѵ�.
	if (!m_Text->SetRenderCount(renderCount, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}
	*/

	// �ؽ�Ʈ�� �������ϱ� ���� ���� �������� �Ҵ�.
	m_Direct3D->EnableAlphaBlending();

	// �ؽ�Ʈ ���ڿ��� ������ �Ѵ�.
	if (!m_Text->Render(m_Direct3D->GetDeviceContext(), worldMatrix, orthoMatrix))
	{
		return false;
	}

	// �ؽ�Ʈ�� ������ �� �� ���� �������� �����Ѵ�.
	m_Direct3D->DisableAlphaBlending();

	m_Direct3D->EndScene();

	return true;
}
