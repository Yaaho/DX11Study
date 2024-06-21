#include "Stdafx.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "LightClass.h"
#include "RenderTextureClass.h"
#include "DepthShaderClass.h"
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
    // Direct3D ��ü ����
    m_D3D = new D3DClass;
    if (!m_D3D)
    {
        return false;
    }

    // Direct3D ��ü �ʱ�ȭ
    bool result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
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

    // ī�޶� �������� �����Ѵ�
    m_Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -10.0f));

    // ť�� �� ������Ʈ�� �����մϴ�.
    m_CubeModel = new ModelClass;
    if (!m_CubeModel)
    {
        return false;
    }

    // ť�� �� ������Ʈ�� �ʱ�ȭ �մϴ�.
    result = m_CubeModel->Initialize(m_D3D->GetDevice(), "data/cube.txt");
    result = m_CubeModel->LoadTextures(m_D3D->GetDevice(), L"data/wall01.dds");

    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
        return false;
    }

    // ť�� ���� ��ġ�� ??���� �մϴ�.
    m_CubeModel->SetPosition(-2.0f, 2.0f, 0.0f);

    // ���� �� ��ü�� ����ϴ�.
    m_SphereModel = new ModelClass;
    if (!m_SphereModel)
    {
        return false;
    }

    // ���� �� ��ü�� �ʱ�ȭ�մϴ�.
    result = m_SphereModel->Initialize(m_D3D->GetDevice(), "data/sphere.txt");
    result = m_SphereModel->LoadTextures(m_D3D->GetDevice(), L"data/ice.dds");

    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK);
        return false;
    }

    // ���� ���� ��ġ�� ??�����մϴ�.
    m_SphereModel->SetPosition(2.0f, 2.0f, 0.0f);

    // ���� �� ��ü�� ����ϴ�.
    m_GroundModel = new ModelClass;
    if (!m_GroundModel)
    {
        return false;
    }

    // ���� �� ��ü�� �ʱ�ȭ�մϴ�.
    result = m_GroundModel->Initialize(m_D3D->GetDevice(), "data/plane01.txt");
    result = m_GroundModel->LoadTextures(m_D3D->GetDevice(), L"data/metal001.dds");

    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
        return false;
    }

    // ���� ���� ��ġ�� ??�����մϴ�.
    m_GroundModel->SetPosition(0.0f, 1.0f, 0.0f);

    // light ��ü�� ����ϴ�.
    m_Light = new LightClass;
    if (!m_Light)
    {
        return false;
    }

    // ���� ��ü�� �ʱ�ȭ�մϴ�.
    m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->GenerateOrthoMatrix(20.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);

    // �������� �ؽ�ó ������Ʈ�� �����Ѵ�.
    m_RenderTexture = new RenderTextureClass;
    if (!m_RenderTexture)
    {
        return false;
    }

    // �������� �ؽ�ó ������Ʈ�� �ʱ�ȭ�Ѵ�.
    result = m_RenderTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT,
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
    result = m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd);
    if (!result)
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
    result = m_ShadowShader->Initialize(m_D3D->GetDevice(), hwnd);
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
    if (m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    // ���� �� ��ü�� �����մϴ�.
    if (m_GroundModel)
    {
        m_GroundModel->Shutdown();
        delete m_GroundModel;
        m_GroundModel = 0;
    }

    // ���� �� ��ü�� �����մϴ�.
    if (m_SphereModel)
    {
        m_SphereModel->Shutdown();
        delete m_SphereModel;
        m_SphereModel = 0;
    }

    // ť�� �� ��ü�� �����մϴ�.
    if (m_CubeModel)
    {
        m_CubeModel->Shutdown();
        delete m_CubeModel;
        m_CubeModel = 0;
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
}

bool ApplicationClass::Frame(float frameTime)
{
    static float lightAngle = 270.0f;
    static float lightPosX = 9.0f;
 
    // �� �������� ���� ��ġ�� ������Ʈ �մϴ�.
    lightPosX -= 0.003f * frameTime;
 
    // �� �������� ���� ������ ������Ʈ �մϴ�.
    lightAngle -= 0.03f * frameTime;
    if(lightAngle < 90.0f)
    {
        lightAngle = 270.0f;
 
        // ���� ��ġ�� �缳�� �մϴ�.
        lightPosX = 9.0f;
    }
    float radians = lightAngle * 0.0174532925f;
 
    // ���� ������ ������ Ʈ�մϴ�.
    m_Light->SetDirection(sinf(radians), cosf(radians), 0.0f);
 
    // ���� ��ġ�� ǥ�ø� �����մϴ�.
    m_Light->SetPosition(lightPosX, 8.0f, -0.1f);
    m_Light->SetLookAt(-lightPosX, 0.0f, 0.0f);
    
    // �׷��� ����� �������մϴ�.
    return Render();


    /*
    // ī�޶� ��ġ�� �����մϴ�.
    m_Camera->SetPosition(XMFLOAT3(posX, posY, posZ));
    m_Camera->SetRotation(XMFLOAT3(rotX, rotY, rotZ));
    */


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


bool ApplicationClass::RenderSceneToTexture()
{
    XMMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix;

    float posX = 0;
    float posY = 0;
    float posZ = 0;

    // ������ ����� �������� �°� �����մϴ�.
    m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext());

    // �������� �ؽ�ó�� ����ϴ�.
    m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    // ������ ��ġ�� ���� ������ ����� �����մϴ�.
    m_Light->GenerateViewMatrix();

    // d3d ��ü���� ���� ����� �����ɴϴ�.
    m_D3D->GetWorldMatrix(worldMatrix);

    // ����Ʈ ������Ʈ���� �� �� ���簢�� ��Ʈ������ �����ɴϴ�.
    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    // ť�� �𵨿� ���� ��ȯ ����� �����Ͻʽÿ�.
    m_CubeModel->GetPosition(posX, posY, posZ);
    worldMatrix = XMMatrixTranslation(posX, posY, posZ);

    // ���� ���̴��� ť�� ���� �������մϴ�.
    m_CubeModel->Render(m_D3D->GetDeviceContext());
    bool result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix,
        lightViewMatrix, lightOrthoMatrix);
    if (!result)
    {
        return false;
    }

    // ���� ����� �缳���մϴ�.
    m_D3D->GetWorldMatrix(worldMatrix);

    // ���� �𵨿� ���� ��ȯ ����� �����մϴ�.
    m_SphereModel->GetPosition(posX, posY, posZ);
    worldMatrix = XMMatrixTranslation(posX, posY, posZ);

    // ���� ���̴��� ���� ���� �������մϴ�.
    m_SphereModel->Render(m_D3D->GetDeviceContext());
    result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix,
        lightViewMatrix, lightOrthoMatrix);
    if (!result)
    {
        return false;
    }

    // ���� ����� �缳���մϴ�.
    m_D3D->GetWorldMatrix(worldMatrix);

    // ground �𵨿� ���� ��ȯ ����� �����մϴ�.
    m_GroundModel->GetPosition(posX, posY, posZ);
    worldMatrix = XMMatrixTranslation(posX, posY, posZ);

    // ���� ���̴��� �׶��� ���� �������մϴ�.
    m_GroundModel->Render(m_D3D->GetDeviceContext());
    result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix,
        lightViewMatrix, lightOrthoMatrix);
    if (!result)
    {
        return false;
    }

    // ������ ����� ������ �� ���۷� �ٽ� �����ϰ� �������� ���� �������� �� �̻� �ٽ� �������� �ʽ��ϴ�.
    m_D3D->SetBackBufferRenderTarget();

    // ����Ʈ�� �������� �ٽ� �����մϴ�.
    m_D3D->ResetViewport();

    return true;
}


bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    XMMATRIX lightViewMatrix, lightOrthoMatrix;

    float posX = 0;
    float posY = 0;
    float posZ = 0;

    // ���� ����� �ؽ�ó�� �������մϴ�.
    bool result = RenderSceneToTexture();
    if (!result)
    {
        return false;
    }

    // ����� ������ ���۸� �����.
    m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
    m_Camera->Render();

    // ������ ��ġ�� ���� ������ ����� �����մϴ�.
    m_Light->GenerateViewMatrix();

    // ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
    m_Camera->GetViewMatrix(viewMatrix);
    m_D3D->GetWorldMatrix(worldMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);

    // ����Ʈ ������Ʈ�κ��� ����Ʈ�� ��� ���� ����� �����ɴϴ�.
    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    // ť�� �𵨿� ���� ��ȯ ����� �����Ͻʽÿ�.
    m_CubeModel->GetPosition(posX, posY, posZ);
    worldMatrix = XMMatrixTranslation(posX, posY, posZ);

    // ť�� �� ������ �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� �׸��⸦ �غ��մϴ�.
    m_CubeModel->Render(m_D3D->GetDeviceContext());

    // �׸��� ���̴��� ����Ͽ� ���� �������մϴ�.
    result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_CubeModel->GetTexture(0),
        m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(),
        m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

    // ���� ����� �缳���մϴ�.
    m_D3D->GetWorldMatrix(worldMatrix);

    // ���� �𵨿� ���� ��ȯ ����� �����մϴ�.
    m_SphereModel->GetPosition(posX, posY, posZ);
    worldMatrix = XMMatrixTranslation(posX, posY, posZ);

    // �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��մϴ�.
    m_SphereModel->Render(m_D3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_SphereModel->GetTexture(0),
        m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(),
        m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

    // ���� ����� �缳���մϴ�.
    m_D3D->GetWorldMatrix(worldMatrix);

    // ground �𵨿� ���� ��ȯ ����� �����մϴ�.
    m_GroundModel->GetPosition(posX, posY, posZ);
    worldMatrix = XMMatrixTranslation(posX, posY, posZ);

    // �׸��� ���̴��� ����Ͽ� �׶��� ���� �������մϴ�.
    m_GroundModel->Render(m_D3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_GroundModel->GetTexture(0),
        m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(),
        m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

    // ������ �� ����� ȭ�鿡 ǥ���մϴ�.
    m_D3D->EndScene();

    return true;
}


