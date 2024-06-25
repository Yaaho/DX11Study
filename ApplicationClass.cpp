#include "Stdafx.h"
#include "InputClass.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "LightClass.h"
#include "ModelClass.h"
#include "OrthoWindowClass.h"
#include "DeferredBuffersClass.h"
#include "DeferredShaderClass.h"
#include "LightShaderClass.h"
#include "ApplicationClass.h"


ApplicationClass::ApplicationClass()
{
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
    bool result;


    // �Է� ��ü�� ����ϴ�. �Է� ��ü�� ����ڷκ��� Ű���� �� ���콺 �Է� �б⸦ ó���ϴ� �� ���˴ϴ�.
    m_Input = new InputClass;
    if (!m_Input)
    {
        return false;
    }

    // �Է� ��ü�� �ʱ�ȭ �մϴ�.
    result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
        return false;
    }

    // Direct3D ��ü�� �����մϴ�.
    m_D3D = new D3DClass;
    if (!m_D3D)
    {
        return false;
    }

    // Direct3D ��ü�� �ʱ�ȭ �մϴ�.
    result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
        return false;
    }

    // ī�޶� ��ü�� �����մϴ�.
    m_Camera = new CameraClass;
    if (!m_Camera)
    {
        return false;
    }

    // ī�޶��� �ʱ� ��ġ�� �����ϰ� �������� �ʿ��� ����� ����ϴ�.
    m_Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -5.0f));
    m_Camera->Render();
    m_Camera->RenderBaseViewMatrix();

    // ���� ��ü�� �����մϴ�.
    m_Light = new LightClass;
    if (!m_Light)
    {
        return false;
    }

    // ���� ��ü�� �ʱ�ȭ �մϴ�.
    m_Light->SetDiffuseColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    m_Light->SetDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));



    // �� ��ü�� �����մϴ�.
    m_Model = new ModelClass;
    if (!m_Model)
    {
        return false;
    }


    // �׶��� ���� ��ġ�� ?�����մϴ�.
    result = m_Model->Initialize(m_D3D->GetDevice(), "data/cube.txt", 1.0f);
    result = m_Model->LoadTextures(m_D3D->GetDevice(), L"data/seafloor.dds");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    // ��ü ȭ�� ortho window ��ü�� �����մϴ�.
    m_FullScreenWindow = new OrthoWindowClass;
    if (!m_FullScreenWindow)
    {
        return false;
    }

    // ��ü ȭ�� ortho window ��ü�� �ʱ�ȭ �մϴ�.
    result = m_FullScreenWindow->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
        return false;
    }

    // ������ ���� ��ü�� �����մϴ�.
    m_DeferredBuffers = new DeferredBuffersClass;
    if (!m_DeferredBuffers)
    {
        return false;
    }

    // ������ ���� ��ü�� �ʱ�ȭ �մϴ�.
    result = m_DeferredBuffers->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the deferred buffers object.", L"Error", MB_OK);
        return false;
    }

    // ������ ���̴� ��ü�� �����մϴ�.
    m_DeferredShader = new DeferredShaderClass;
    if (!m_DeferredShader)
    {
        return false;
    }

    // ������ ���̴� ��ü�� �ʱ�ȭ �մϴ�.
    result = m_DeferredShader->Initialize(m_D3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the deferred shader object.", L"Error", MB_OK);
        return false;
    }

    // ���� ���̴� ��ü�� �����մϴ�.
    m_LightShader = new LightShaderClass;
    if (!m_LightShader)
    {
        return false;
    }

    // ���� ���̴� ��ü�� �ʱ�ȭ �մϴ�.
    result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
        return false;
    }

    return true;
}


void ApplicationClass::Shutdown()
{
    // ���� ���̴� ��ü�� �����մϴ�.
    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
    }

    // ������ ���̴� ��ü�� �����մϴ�.
    if (m_DeferredShader)
    {
        m_DeferredShader->Shutdown();
        delete m_DeferredShader;
        m_DeferredShader = 0;
    }

    // ������ ���� ��ü�� �����մϴ�.
    if (m_DeferredBuffers)
    {
        m_DeferredBuffers->Shutdown();
        delete m_DeferredBuffers;
        m_DeferredBuffers = 0;
    }

    // ��ü ȭ�� ortho window ��ü�� �����մϴ�.
    if (m_FullScreenWindow)
    {
        m_FullScreenWindow->Shutdown();
        delete m_FullScreenWindow;
        m_FullScreenWindow = 0;
    }

    // �� ��ü�� �����մϴ�.
    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
    }

    // ���� ��ü�� �����մϴ�.
    if (m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    // ī�޶� ��ü�� �����մϴ�.
    if (m_Camera)
    {
        delete m_Camera;
        m_Camera = 0;
    }

    // D3D ��ü�� �����մϴ�.
    if (m_D3D)
    {
        m_D3D->Shutdown();
        delete m_D3D;
        m_D3D = 0;
    }

    // �Է� ��ü�� �����մϴ�.
    if (m_Input)
    {
        m_Input->Shutdown();
        delete m_Input;
        m_Input = 0;
    }
}

bool ApplicationClass::Frame()
{
    // ����� �Է��� �н��ϴ�.
    if (!m_Input->Frame())
    {
        return false;
    }

    // ����ڰ� ESC Ű�� ������ ���� ���α׷��� ������ ������ Ȯ���մϴ�.
    if (m_Input->IsEscapePressed() == true)
    {
        return false;
    }

    // �׷����� ������ �մϴ�.
    return Render();

    /*
    �ʴ� ������ ���� �����Ѵ�.
    if (!m_Text->SetFps(fps, m_Direct3D->GetDeviceContext()))
    {
        return false;
    }

    cpu ����� �����Ѵ�.
    if (!m_Text->SetCpu(cpu, m_Direct3D->GetDeviceContext()))
    {
        return false;
    }
    */
}


bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

    // ���� ����� �ؽ�ó�� �������մϴ�.
    if (!RenderSceneToTexture())
    {
        return false;
    }

    // ����� ������ ���۸� �����.
    m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
    m_D3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetBaseViewMatrix(baseViewMatrix);
    m_D3D->GetOrthoMatrix(orthoMatrix);

    // ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
    m_D3D->TurnZBufferOff();

    // �׷��� ������ ���ο� ��ü ȭ�� ���� ������ ���ؽ��� �ε��� ���۸� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
    m_FullScreenWindow->Render(m_D3D->GetDeviceContext());

    // ���� ���� ���̴��� ������ ���۸� ����Ͽ� ��ü ȭ�� ���翵 �����츦 �������մϴ�.
    m_LightShader->Render(m_D3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix,
        orthoMatrix, m_DeferredBuffers->GetShaderResourceView(0), m_DeferredBuffers->GetShaderResourceView(1),
        m_Light->GetDirection());

    // ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �մϴ�.
    m_D3D->TurnZBufferOn();

    // ������ �� ����� ȭ�鿡 ǥ���մϴ�.
    m_D3D->EndScene();

    return true;
}


bool ApplicationClass::RenderSceneToTexture()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

    // ������ ����� �ؽ�ó�� ���������� �����մϴ�.
    m_DeferredBuffers->SetRenderTargets(m_D3D->GetDeviceContext());

    // �������� �ؽ�ó�� ����ϴ�.
    m_DeferredBuffers->ClearRenderTargets(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    // d3d ��ü���� ���� ����� �����ɴϴ�.
    m_D3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);

    // �� �������� rotation ������ ������Ʈ�մϴ�.
    static float rotation = 0.0f;
    rotation += (float)XM_PI * 0.007f;
    if (rotation > 360.0f)
    {
        rotation -= 360.0f;
    }

    // ȸ�������� ���� ����� ȸ���Ͽ� ť�갡 ȸ���ϵ��� �մϴ�.
    worldMatrix = XMMatrixRotationY(rotation);

    // �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��մϴ�.
    m_Model->Render(m_D3D->GetDeviceContext());

    // ������ ���̴��� ����Ͽ� ���� �������մϴ�.
    m_DeferredShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Model->GetTexture(0));

    // ���� ���۰� �ƴ� ������ �� ���۷� ������ Ÿ���� �ٽ� �����մϴ�.
    m_D3D->SetBackBufferRenderTarget();

    // ����Ʈ�� �������� �ٽ� �����մϴ�.
    m_D3D->ResetViewport();

    return true;
}
