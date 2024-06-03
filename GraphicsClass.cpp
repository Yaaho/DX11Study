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
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
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


	// m_Light ��ü ����
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// m_Light ��ü �ʱ�ȭ
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f);


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






	// ����ŧ�� �� ���̴� ��ü ����
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// ����ŧ�� �� ���̴� �ʱ�ȭ
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

	// m_AlphaMapShader ��ü ��ȯ
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// ���� �ý��� ��ü ��ȯ
	if (m_ReflectionTexture)
	{
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = 0;
	}

	// ���� �ý��� ��ü ��ȯ
	if (m_RefractionTexture)
	{
		m_RefractionTexture->Shutdown();
		delete m_RefractionTexture;
		m_RefractionTexture = 0;
	}


	// ���� �ý��� ��ü ��ȯ
	if (m_FadeRenderTexture)
	{
		m_FadeRenderTexture->Shutdown();
		delete m_FadeRenderTexture;
		m_FadeRenderTexture = 0;
	}



	// m_Light ��ü ��ȯ
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}



	// m_Model ��ü ��ȯ
	if (m_WaterModel)
	{
		m_WaterModel->Shutdown();
		delete m_WaterModel;
		m_WaterModel = 0;
	}

	// m_Model ��ü ��ȯ
	if (m_BathModel)
	{
		m_BathModel->Shutdown();
		delete m_BathModel;
		m_BathModel = 0;
	}

	// m_Model ��ü ��ȯ
	if (m_WallModel)
	{
		m_WallModel->Shutdown();
		delete m_WallModel;
		m_WallModel = 0;
	}

	// m_Model ��ü ��ȯ
	if (m_GroundModel)
	{
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
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

bool GraphicsClass::Frame(float rotationY, float frameTime)
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

	// ���� �������� ������Ʈ �Ѵ�.
	m_waterTranslation += 0.001f;
	if (m_waterTranslation > 1.0f)
	{
		m_waterTranslation -= 1.0f;
	}


	// ī�޶��� ��ġ�� �����Ѵ�.
	m_Camera->SetPosition(-11.0f, 6.0f, -10.0f);

	// ī�޶��� ȸ���� �����Ѵ�.
	m_Camera->SetRotation(0.0f, rotationY, 0.0f);


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
	bool result;
	int renderCount;

	RenderRefractionToTexture();
	RenderReflectionToTexture();

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

	// ����� ������ ���۸� �����.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� (ortho) ����� �����ɴϴ�.
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
	// ������ ����� �������� �°� �����մϴ�.
	m_FadeRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// �������� �ؽ�ó�� �� ��ϴ�.
	m_FadeRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	int renderCount = RenderScene();

	// ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
	m_Direct3D->SetBackBufferRenderTarget();

	return renderCount;
}



bool GraphicsClass::RenderRefractionToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// �� ǥ�� ���� �ִ� ���� ��� Ŭ�����ϵ��� clipPlane ����
	XMFLOAT4 clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, m_waterHeight + 0.1f);

	// ������ ����� �������� �°� �����մϴ�.
	m_RefractionTexture->SetRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView());

	// �������� �ؽ�ó�� �� ��ϴ�.
	m_RefractionTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), m_Direct3D->GetDepthStencilView(), 0.0f, 0.0f,
		0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// ���� ���� �������Ǵ� ��ġ�� tranlation 
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);

	// ���� ���� ���ؽ��� �������� �� �ֵ��� ������ �׷��� ���������ο� ��ġ
	m_BathModel->Render(m_Direct3D->GetDeviceContext());

	// ���� �Ʒ��� �׷������� ������ m_RefractionShader ���� ������
	if (!m_RefractionShader->Render(m_Direct3D->GetDeviceContext(), m_BathModel->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix, m_BathModel->GetTexture(0), m_Light->GetDirection(),
		m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), clipPlane))
	{
		return false;
	}

	// ���� Ÿ���� ������ �� ���۷� ����
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