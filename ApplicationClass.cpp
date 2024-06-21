#include "Stdafx.h"
#include "InputClass.h"
#include "D3DClass.h"
#include "TimerClass.h"
#include "PositionClass.h"
#include "CameraClass.h"
#include "LightClass.h"
#include "ModelClass.h"
#include "TreeClass.h"
#include "RenderTextureClass.h"
#include "DepthShaderClass.h"
#include "TransparentDepthShaderClass.h"
#include "ShadowShaderClass.h"
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

    // Direct3D ��ü�� ����ϴ�.
    m_Direct3D = new D3DClass;
    if (!m_Direct3D)
    {
        return false;
    }

    // Direct3D ��ü�� �ʱ�ȭ �մϴ�.
    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
        return false;
    }

    // Ÿ�̸� ��ü�� ����ϴ�.
    m_Timer = new TimerClass;
    if (!m_Timer)
    {
        return false;
    }

    // Ÿ�̸� ��ü�� �ʱ�ȭ �մϴ�.
    result = m_Timer->Initialize();
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
        return false;
    }

    // ��ġ ��ü�� ����ϴ�.
    m_Position = new PositionClass;
    if (!m_Position)
    {
        return false;
    }

    // �ʱ� ��ġ�� �����մϴ�.
    m_Position->SetPosition(0.0f, 7.0f, -11.0f);
    m_Position->SetRotation(20.0f, 0.0f, 0.0f);

    // ī�޶� ��ü�� ����ϴ�.
    m_Camera = new CameraClass;
    if (!m_Camera)
    {
        return false;
    }

    // ���� ��ü�� ����ϴ�.
    m_Light = new LightClass;
    if (!m_Light)
    {
        return false;
    }

    // ���� ��ü�� �ʱ�ȭ �մϴ�.
    m_Light->GenerateOrthoMatrix(15.0f, 15.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);

    // �׶��� �� ��ü�� ����ϴ�.
    m_GroundModel = new ModelClass;
    if (!m_GroundModel)
    {
        return false;
    }

    // �׶��� ���� ��ġ�� ?�����մϴ�.
    result = m_GroundModel->Initialize(m_Direct3D->GetDevice(), "data/plane01.txt", 2.0f);
    result = m_GroundModel->LoadTextures(m_Direct3D->GetDevice(), L"data/dirt.dds");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
        return false;
    }

    // ���� �� ��ġ ��ġ�� �����մϴ�.
    m_GroundModel->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));

    // ���� ��ü�� ����ϴ�.
    m_Tree = new TreeClass;
    if (!m_Tree)
    {
        return false;
    }

    // �׸��� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
    result = m_Tree->Initialize(m_Direct3D->GetDevice(), "data/trees/trunk001.txt",
        L"data/trees/trunk001.dds", "data/trees/leaf001.txt",
        L"data/trees/leaf001.dds", 0.1f);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the tree object.", L"Error", MB_OK);
        return false;
    }

    // ���� ���� ��ġ�� ??�����մϴ�.
    m_Tree->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));

    // �ؽ�ó ��ü�� �������� ����ϴ�.
    m_RenderTexture = new RenderTextureClass;
    if (!m_RenderTexture)
    {
        return false;
    }

    // �������� �ؽ�ó ��ü�� �ʱ�ȭ�մϴ�.
    result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT,
        SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
        return false;
    }

    // ���� ���̴� ��ü�� ����ϴ�.
    m_DepthShader = new DepthShaderClass;
    if (!m_DepthShader)
    {
        return false;
    }

    // ���� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
    result = m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
        return false;
    }

    // ������ ���� ���̴� ��ü�� ����ϴ�.
    m_TransparentDepthShader = new TransparentDepthShaderClass;
    if (!m_TransparentDepthShader)
    {
        return false;
    }

    // ������ ���� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
    result = m_TransparentDepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the transparent depth shader object.", L"Error", MB_OK);
        return false;
    }

    // �׸��� ���̴� ��ü�� ����ϴ�.
    m_ShadowShader = new ShadowShaderClass;
    if (!m_ShadowShader)
    {
        return false;
    }

    // �׸��� ���̴� ��ü�� �ʱ�ȭ�մϴ�.
    result = m_ShadowShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
        return false;
    }

    return true;
}


void ApplicationClass::Shutdown()
{
    // �׸��� ���̴� ��ü�� �����մϴ�.
    if (m_ShadowShader)
    {
        m_ShadowShader->Shutdown();
        delete m_ShadowShader;
        m_ShadowShader = 0;
    }

    // ������ ���� ���̴� ��ü�� �����մϴ�.
    if (m_TransparentDepthShader)
    {
        m_TransparentDepthShader->Shutdown();
        delete m_TransparentDepthShader;
        m_TransparentDepthShader = 0;
    }

    // ���� ���̴� ��ü�� �����մϴ�.
    if (m_DepthShader)
    {
        m_DepthShader->Shutdown();
        delete m_DepthShader;
        m_DepthShader = 0;
    }

    // ���� �� �ؽ��� ��ü�� �����մϴ�.
    if (m_RenderTexture)
    {
        m_RenderTexture->Shutdown();
        delete m_RenderTexture;
        m_RenderTexture = 0;
    }

    // ���� ��ü�� �����մϴ�.
    if (m_Tree)
    {
        m_Tree->Shutdown();
        delete m_Tree;
        m_Tree = 0;
    }

    // ���� �� ��ü�� �����մϴ�.
    if (m_GroundModel)
    {
        m_GroundModel->Shutdown();
        delete m_GroundModel;
        m_GroundModel = 0;
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

    // ��ġ ��ü�� �����մϴ�.
    if (m_Position)
    {
        delete m_Position;
        m_Position = 0;
    }

    // Ÿ�̸� ��ü�� �����մϴ�.
    if (m_Timer)
    {
        delete m_Timer;
        m_Timer = 0;
    }

    // Direct3D ��ü�� �����մϴ�.
    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = 0;
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
    // �ý��� ��踦 ������Ʈ �մϴ�.
    m_Timer->Frame();

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

    // ������ �Է� ó���� �����մϴ�.
    if (!HandleMovementInput(m_Timer->GetTime()))
    {
        return false;
    }

    // ����� ������ ������Ʈ �մϴ�.
    UpdateLighting();

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


bool ApplicationClass::HandleMovementInput(float frameTime)
{
    XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3 rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

    // ���ŵ� ��ġ�� ����ϱ� ���� ������ �ð��� �����մϴ�.
    m_Position->SetFrameTime(frameTime);

    // �Է��� ó���մϴ�.
    m_Position->TurnLeft(m_Input->IsLeftPressed());
    m_Position->TurnRight(m_Input->IsRightPressed());
    m_Position->MoveForward(m_Input->IsUpPressed());
    m_Position->MoveBackward(m_Input->IsDownPressed());
    m_Position->MoveUpward(m_Input->IsAPressed());
    m_Position->MoveDownward(m_Input->IsZPressed());
    m_Position->LookUpward(m_Input->IsPgUpPressed());
    m_Position->LookDownward(m_Input->IsPgDownPressed());

    // ���� ��ġ / ȸ���� �����ɴϴ�.
    m_Position->GetPosition(pos);
    m_Position->GetRotation(rot);

    // ī�޶��� ��ġ�� ??�����մϴ�.
    m_Camera->SetPosition(pos);
    m_Camera->SetRotation(rot);

    return true;
}


void ApplicationClass::UpdateLighting()
{
    static float angle = 270.0f;
    static float offsetX = 9.0f;

    // ���� ������ ������Ʈ �մϴ�.
    angle -= 0.03f * m_Timer->GetTime();
    if (angle < 90.0f)
    {
        angle = 270.0f;
        offsetX = 9.0f;
    }
    float radians = angle * 0.0174532925f;
    m_Light->SetDirection(XMFLOAT3(sinf(radians), cosf(radians), 0.0f));

    // ��ȸ �� ��ġ�� ������Ʈ �մϴ�.
    offsetX -= 0.003f * m_Timer->GetTime();
    m_Light->SetPosition(XMFLOAT3(0.0f + offsetX, 10.0f, 1.0f));
    m_Light->SetLookAt(XMFLOAT3(0.0f - offsetX, 0.0f, 2.0f));
}


bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightOrthoMatrix;
    XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);

    // ���� ����� �ؽ�ó�� �������մϴ�.
    if (!RenderSceneToTexture())
    {
        return false;
    }

    // ����� ������ ���۸� �����.
    m_Direct3D->BeginScene(0.0f, 0.5f, 0.8f, 1.0f);

    // ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
    m_Camera->Render();

    // ������ ��ġ�� ���� ������ ����� �����մϴ�.
    m_Light->GenerateViewMatrix();

    // ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    // ����Ʈ ������Ʈ�κ��� ����Ʈ�� ��� ���� ����� �����ɴϴ�.
    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    // ���� ���� �Ӽ��� �����մϴ�.
    XMFLOAT4 diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    XMFLOAT4 ambientColor = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

    // ���� ���� ��ġ�� ��ȯ�մϴ�.
    m_GroundModel->GetPosition(pos);
    worldMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

    // �׸��� ���̴��� ����Ͽ� �׶��� ���� ������ �մϴ�.
    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_GroundModel->GetTexture(0),
        m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(),
        ambientColor, diffuseColor);
    m_Direct3D->GetWorldMatrix(worldMatrix);

    // Ʈ�� ���� ��ġ�� ��ȯ�մϴ�.
    m_Tree->GetPosition(pos);
    worldMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

    // ���� �ٱ⸦ ������ �մϴ�.
    m_Tree->RenderTrunk(m_Direct3D->GetDeviceContext());
    m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Tree->GetTrunkIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_Tree->GetTrunkTexture(),
        m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(),
        ambientColor, diffuseColor);

    // ������ Ȱ��ȭ�ϰ� ������ ������ �մϴ�.
    m_Direct3D->EnableAlphaBlending();
    m_Tree->RenderLeaves(m_Direct3D->GetDeviceContext());
    m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Tree->GetLeafIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_Tree->GetLeafTexture(),
        m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(), ambientColor,
        diffuseColor);

    m_Direct3D->DisableAlphaBlending();
    m_Direct3D->GetWorldMatrix(worldMatrix);

    // ������ �� ����� ȭ�鿡 ����մϴ�.
    m_Direct3D->EndScene();

    return true;
}


bool ApplicationClass::RenderSceneToTexture()
{
    XMMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix;
    XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);

    // ������ ����� �ؽ�ó�� ���������� �����մϴ�.
    m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

    // �������� �ؽ�ó�� ����ϴ�.
    m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    // d3d ��ü���� ���� ����� �����ɴϴ�.
    m_Direct3D->GetWorldMatrix(worldMatrix);

    // ������ ��ġ�� ���� ������ ����� �����մϴ�.
    m_Light->GenerateViewMatrix();

    // ����Ʈ ������Ʈ���� �� �� ���簢�� ��Ʈ������ �����ɴϴ�.
    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    // ������ ��ġ�� ��ȯ�մϴ�.
    m_Tree->GetPosition(pos);
    worldMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

    // ���� ���̴��� ���� Ʈ��ũ�� �������մϴ�.
    m_Tree->RenderTrunk(m_Direct3D->GetDeviceContext());
    m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Tree->GetTrunkIndexCount(), worldMatrix, lightViewMatrix,
        lightOrthoMatrix);

    // ���� ���� ���̴��� ����Ͽ� ������ �������� ������ �մϴ�.
    m_Tree->RenderLeaves(m_Direct3D->GetDeviceContext());
    if (!m_TransparentDepthShader->Render(m_Direct3D->GetDeviceContext(), m_Tree->GetLeafIndexCount(), worldMatrix,
        lightViewMatrix, lightOrthoMatrix, m_Tree->GetLeafTexture()))
    {
        return false;
    }

    // ���� ����� �缳���մϴ�.
    m_Direct3D->GetWorldMatrix(worldMatrix);

    // ground �𵨿� ���� ��ȯ ����� �����մϴ�.
    m_GroundModel->GetPosition(pos);
    worldMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

    // �׸��� ���̴��� ����Ͽ� �׶��� ���� �������մϴ�.
    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, lightViewMatrix,
        lightOrthoMatrix);

    // ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
    m_Direct3D->SetBackBufferRenderTarget();

    // ����Ʈ�� �������� �ٽ� �����մϴ�.
    m_Direct3D->ResetViewport();

    return true;
}
