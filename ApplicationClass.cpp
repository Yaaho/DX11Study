#include "Stdafx.h"
#include "InputClass.h"
#include "D3DClass.h"
#include "TimerClass.h"
#include "PositionClass.h"
#include "CameraClass.h"
#include "LightClass.h"
#include "SamplerClass.h"
#include "ModelClass.h"
#include "OrthoWindowClass.h"
#include "SkyboxClass.h"
#include "TextureClass.h"
#include "RenderTextureClass.h"
#include "RenderTextureCubeClass.h"
#include "DepthShaderClass.h"
#include "GBuffersClass.h"
#include "GBufferShaderClass.h"
#include "DeferredShaderClass.h"
#include "SkyboxShaderClass.h"
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



    // ī�޶� ��ü�� �����մϴ�.
    m_Camera = new CameraClass;
    if (!m_Camera)
    {
        return false;
    }

    XMFLOAT3 pos;

    m_Position->GetPosition(pos);

    // ī�޶��� �ʱ� ��ġ�� �����ϰ� �������� �ʿ��� ����� ����ϴ�.
    m_Camera->SetPosition(pos);
    m_Camera->Render();
    m_Camera->RenderBaseViewMatrix();

    // ���� ��ü�� �����մϴ�.
    m_Light = new LightClass;
    if (!m_Light->Initialize(m_Direct3D->GetDevice(), hwnd, 4096, 4096, SCREEN_DEPTH, SCREEN_NEAR))
    {
        return false;
    }

    m_Light->m_lightProps.m_GlobalAmbient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);

    m_Light->m_lightProps.m_Lights[0].m_Position = XMFLOAT4(0.0f, 7.0f, -10.0f, 1.0f);
    m_Light->m_lightProps.m_Lights[1].m_Position = XMFLOAT4(7.0f, 5.0f, 7.0f, 1.0f);
    m_Light->m_lightProps.m_Lights[2].m_Position = XMFLOAT4(-4.0f, 5.0f, -5.0f, 1.0f);
    m_Light->m_lightProps.m_Lights[3].m_Position = XMFLOAT4(-9.0f, 3.0f, -8.0f, 1.0f);


    m_Light->m_lightProps.m_Lights[0].m_Direction = XMFLOAT4(0.0f, -0.5f, 0.5f, 1.0f);
    m_Light->m_lightProps.m_Lights[1].m_Direction = XMFLOAT4(-7.0f, -5.0f, -7.0f, 1.0f);
    m_Light->m_lightProps.m_Lights[2].m_Direction = XMFLOAT4(4.0f, -5.0f, 5.0f, 1.0f);
    m_Light->m_lightProps.m_Lights[3].m_Direction = XMFLOAT4(9.0f, -3.0f, 8.0f, 1.0f);


    m_Light->m_lightProps.m_Lights[0].m_Color = XMFLOAT4(0.7f, 0.4f, 0.2f, 1.0f);
    m_Light->m_lightProps.m_Lights[1].m_Color = XMFLOAT4(0.4f, 0.9f, 0.6f, 1.0f);
    m_Light->m_lightProps.m_Lights[2].m_Color = XMFLOAT4(0.7f, 0.8f, 0.9f, 1.0f);
    m_Light->m_lightProps.m_Lights[3].m_Color = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);

    m_Light->m_lightProps.m_Lights[0].m_LightType = LightType::DirectionalLight;
    m_Light->m_lightProps.m_Lights[1].m_LightType = LightType::PointLight;
    m_Light->m_lightProps.m_Lights[2].m_LightType = LightType::PointLight;
    m_Light->m_lightProps.m_Lights[3].m_LightType = LightType::PointLight;


    m_Light->m_lightProps.m_Lights[0].m_Status = LightStatus::Static_Shadow;
    m_Light->m_lightProps.m_Lights[1].m_Status = LightStatus::Enable;
    m_Light->m_lightProps.m_Lights[2].m_Status = LightStatus::Enable;
    m_Light->m_lightProps.m_Lights[3].m_Status = LightStatus::Enable;

    m_Light->m_shadowMapProps.m_ShadowMaps[0].m_ShadowMapTopLeftX = 0.0f;
    m_Light->m_shadowMapProps.m_ShadowMaps[0].m_ShadowMapTopLeftY = 0.0f;
    m_Light->m_shadowMapProps.m_ShadowMaps[0].m_ShadowMapTextureRatio = 0.5f;

    m_Light->m_shadowMapProps.m_ShadowMaps[1].m_ShadowMapTopLeftX = 0.5f;
    m_Light->m_shadowMapProps.m_ShadowMaps[1].m_ShadowMapTopLeftY = 0.0f;
    m_Light->m_shadowMapProps.m_ShadowMaps[1].m_ShadowMapTextureRatio = 0.5f;

    m_Light->m_shadowMapProps.m_ShadowMaps[2].m_ShadowMapTopLeftX = 0.0f;
    m_Light->m_shadowMapProps.m_ShadowMaps[2].m_ShadowMapTopLeftY = 0.5f;
    m_Light->m_shadowMapProps.m_ShadowMaps[2].m_ShadowMapTextureRatio = 0.5f;

    m_Light->m_shadowMapProps.m_ShadowMaps[3].m_ShadowMapTopLeftX = 0.5f;
    m_Light->m_shadowMapProps.m_ShadowMaps[3].m_ShadowMapTopLeftY = 0.5f;
    m_Light->m_shadowMapProps.m_ShadowMaps[3].m_ShadowMapTextureRatio = 0.5f;


















    m_Sampler = new SamplerClass;
    if (!m_Sampler->Initialize(m_Direct3D->GetDevice()))
    {
        return false;
    }


    // �� ��ü�� �����մϴ�.
    m_Cube = new ModelClass;
    if (!m_Cube)
    {
        return false;
    }


    // �׶��� ���� ��ġ�� ?�����մϴ�.
    result = m_Cube->Initialize(m_Direct3D->GetDevice(), "data/cube.txt", 1.0f);
    // 1 �˺���, 2 �븻, 3 ��Ŭ���� �����Ͻ� ��Ż, 4 AO, 5 �̹̽ú�
    result = m_Cube->LoadTextures(m_Direct3D->GetDevice(), 
        L"data/stone01.dds", L"data/normal.dds", L"data/normal.dds", L"data/normal.dds", L"data/normal.dds");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    m_Cube->SetPosition(XMFLOAT3(-2.0f, 2.0f, 0.0f));


    m_Plane = new ModelClass;
    if (!m_Plane)
    {
        return false;
    }

    result = m_Plane->Initialize(m_Direct3D->GetDevice(), "data/plane01.txt", 1.0f);
    result = m_Plane->LoadTextures(m_Direct3D->GetDevice(),
        L"data/stone01.dds", L"data/normal.dds", L"data/normal.dds", L"data/normal.dds", L"data/normal.dds");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    m_Plane->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));

    m_Skybox = new SkyboxClass;
    if (!m_Skybox)
    {
        return false;
    }

    result = m_Skybox->Initialize(m_Direct3D->GetDevice());
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the skybox object.", L"Error", MB_OK);
        return false;
    }

    m_SkyboxCubeMap = new TextureClass;
    if (!m_SkyboxCubeMap)
    {
        return false;
    }

    result = m_SkyboxCubeMap->Initialize(m_Direct3D->GetDevice(), L"sky.dds");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the skybox texture object.", L"Error", MB_OK);
        return false;
    }

    // �������� �ؽ�ó ������Ʈ�� �����Ѵ�.
    m_DepthMapTexture = new RenderTextureClass;
    if (!m_DepthMapTexture)
    {
        return false;
    }

    // �������� �ؽ�ó ������Ʈ�� �ʱ�ȭ�Ѵ�.
    if (!m_DepthMapTexture->Initialize(m_Direct3D->GetDevice(), 4096, 4096, SCREEN_DEPTH, SCREEN_NEAR))
    {
        MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
        return false;
    }

    m_EnvironmentMap = new RenderTextureCubeClass;
    if (!m_EnvironmentMap)
    {
        return false;
    }

    if (!m_EnvironmentMap->Initialize(m_Direct3D->GetDevice(), 512))
    {
        MessageBox(hwnd, L"Could not initialize the render environment Map object.", L"Error", MB_OK);
        return false;
    }

    m_SpecularMap = new RenderTextureCubeClass;
    if (!m_SpecularMap)
    {
        return false;
    }

    if (!m_SpecularMap->Initialize(m_Direct3D->GetDevice(), 512))
    {
        MessageBox(hwnd, L"Could not initialize the render specular map object.", L"Error", MB_OK);
        return false;
    }

    m_BrdfLUT = new RenderTextureClass;
    if (!m_BrdfLUT)
    {
        return false;
    }

    if (!m_BrdfLUT->Initialize(m_Direct3D->GetDevice(), 512, 512, SCREEN_DEPTH, SCREEN_NEAR))
    {
        MessageBox(hwnd, L"Could not initialize the render brdfLUT object.", L"Error", MB_OK);
        return false;
    }

    m_DepthShader = new DepthShaderClass;
    if (!m_DepthShader)
    {
        return false;
    }

    result = m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
        return false;
    }

    // ������ ���� ��ü�� �����մϴ�.
    m_GBuffers = new GBuffersClass;
    if (!m_GBuffers)
    {
        return false;
    }

    // ������ ���� ��ü�� �ʱ�ȭ �մϴ�.
    result = m_GBuffers->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the deferred buffers object.", L"Error", MB_OK);
        return false;
    }

    // ������ ���̴� ��ü�� �����մϴ�.
    m_GBufferShader = new GBufferShaderClass;
    if (!m_GBufferShader)
    {
        return false;
    }

    // ������ ���̴� ��ü�� �ʱ�ȭ �մϴ�.
    result = m_GBufferShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the gbuffer shader object.", L"Error", MB_OK);
        return false;
    }

    // ���� ���̴� ��ü�� �����մϴ�.
    m_DeferredShader = new DeferredShaderClass;
    if (!m_DeferredShader)
    {
        return false;
    }

    // ���� ���̴� ��ü�� �ʱ�ȭ �մϴ�.
    result = m_DeferredShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the deferred shader object.", L"Error", MB_OK);
        return false;
    }

    m_SkyboxShader = new SkyboxShaderClass;
    if (!m_SkyboxShader)
    {
        return false;
    }

    result = m_SkyboxShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the skybox shader object.", L"Error", MB_OK);
        return false;
    }


    RenderEnvMap();

    return true;
}


void ApplicationClass::Shutdown()
{
    if (m_SkyboxShader)
    {
        m_SkyboxShader->Shutdown();
        delete m_SkyboxShader;
        m_SkyboxShader = 0;
    }


    // ���� ���̴� ��ü�� �����մϴ�.
    if (m_DeferredShader)
    {
        m_DeferredShader->Shutdown();
        delete m_DeferredShader;
        m_DeferredShader = 0;
    }

    // ������ ���̴� ��ü�� �����մϴ�.
    if (m_GBufferShader)
    {
        m_GBufferShader->Shutdown();
        delete m_GBufferShader;
        m_GBufferShader = 0;
    }

    // ������ ���� ��ü�� �����մϴ�.
    if (m_GBuffers)
    {
        m_GBuffers->Shutdown();
        delete m_GBuffers;
        m_GBuffers = 0;
    }

    if (m_DepthShader)
    {
        m_DepthShader->Shutdown();
        delete m_DepthShader;
        m_DepthShader = 0;
    }

    if (m_BrdfLUT)
    {
        m_BrdfLUT->Shutdown();
        delete m_BrdfLUT;
        m_BrdfLUT = 0;
    }

    if (m_SpecularMap)
    {
        m_SpecularMap->Shutdown();
        delete m_SpecularMap;
        m_SpecularMap = 0;
    }

    if (m_EnvironmentMap)
    {
        m_EnvironmentMap->Shutdown();
        delete m_EnvironmentMap;
        m_EnvironmentMap = 0;
    }

    if (m_DepthMapTexture)
    {
        m_DepthMapTexture->Shutdown();
        delete m_DepthMapTexture;
        m_DepthMapTexture = 0;
    }

    if (m_SkyboxCubeMap)
    {
        m_SkyboxCubeMap->Shutdown();
        delete m_SkyboxCubeMap;
        m_SkyboxCubeMap = 0;
    }

    if (m_Skybox)
    {
        m_Skybox->Shutdown();
        delete m_Skybox;
        m_Skybox = 0;
    }

    if (m_Plane)
    {
        m_Plane->Shutdown();
        delete m_Plane;
        m_Plane = 0;
    }

    // �� ��ü�� �����մϴ�.
    if (m_Cube)
    {
        m_Cube->Shutdown();
        delete m_Cube;
        m_Cube = 0;
    }

    if (m_Sampler)
    {
        m_Sampler->Shutdown();
        delete m_Sampler;
        m_Sampler = 0;
    }

    // ���� ��ü�� �����մϴ�.
    if (m_Light)
    {
        m_Light->Shutdown();
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

    // D3D ��ü�� �����մϴ�.
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
    // �ý����� ��踦 ������Ʈ �մϴ�.
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

    // ������ �Է� ó���� �����Ѵ�.
    if (!HandleMovementInput(m_Timer->GetTime()))
    {
        return false;
    }

    if (m_Light)
    {
        m_Light->m_lightProps.m_EyePosition = XMFLOAT4(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z, 1.0f);
        m_Light->UpdateBuffer(m_Direct3D->GetDeviceContext());
    }

    // �׷����� ������ �Ѵ�.
    Render();

    return true;

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

    // ī�޶��� ��ġ�� �����մϴ�.
    m_Camera->SetPosition(pos);
    m_Camera->SetRotation(rot);

    return true;
}



bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, viewMatrix, baseViewMatrix, projectionMatrix, orthoMatrix;

    // ���� ����� �ؽ�ó�� �������մϴ�.
    if (!RenderGBuffer())
    {
        return false;
    }

    // �׸��� �׸��⿡ ����� DepthMap
    if (!RenderDepthMap())
    {
        return false;
    }

    
    // ����� ������ ���۸� �����.
    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Camera->GetBaseViewMatrix(baseViewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    // ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
    m_Direct3D->TurnZBufferOff();


    // s0
    m_Sampler->UseLinear(m_Direct3D->GetDeviceContext(), 0);

    // s1
    m_Sampler->UsePoint(m_Direct3D->GetDeviceContext(), 1);

    // b1
    m_Light->UseLightBuffer(m_Direct3D->GetDeviceContext(), 1);

    // b2
    m_Light->UseShadowMapBuffer(m_Direct3D->GetDeviceContext(), 2);

    // t4
    m_DepthMapTexture->UseShaderResourceView(m_Direct3D->GetDeviceContext(), 4);


    m_EnvironmentMap->UseShaderResourceView(m_Direct3D->GetDeviceContext(), 6);
    m_SpecularMap->UseShaderResourceView(m_Direct3D->GetDeviceContext(), 7);
    m_BrdfLUT->UseShaderResourceView(m_Direct3D->GetDeviceContext(), 8);


    m_Direct3D->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_DeferredShader->Render(m_Direct3D->GetDeviceContext(), 
        XMMatrixInverse(nullptr, projectionMatrix), XMMatrixInverse(nullptr, viewMatrix), false, 1,
        m_GBuffers->GetDepthResourceView(), 
        m_GBuffers->GetShaderResourceView(0), m_GBuffers->GetShaderResourceView(1), m_GBuffers->GetShaderResourceView(2), m_GBuffers->GetShaderResourceView(3));


    // ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
    m_Direct3D->TurnZBufferOn();

    m_Direct3D->SetBackBufferRenderTargetWithStencil(m_GBuffers->GetDepthStencilView());

    RenderSkybox();

    // ������ �� ����� ȭ�鿡 ǥ���մϴ�.
    m_Direct3D->EndScene();

    return true;
}


bool ApplicationClass::RenderGBuffer()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

    XMFLOAT3 Pos;


    // ������ ����� �ؽ�ó�� ���������� �����մϴ�.
    m_GBuffers->SetRenderTargets(m_Direct3D->GetDeviceContext());

    // �������� �ؽ�ó�� ����ϴ�.
    m_GBuffers->ClearRenderTargets(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_Camera->Render();

    // d3d ��ü���� ���� ����� �����ɴϴ�.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    

    m_Cube->GetPosition(Pos);
    worldMatrix = XMMatrixTranslation(Pos.x, Pos.y, Pos.z);


    // �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��մϴ�.
    m_Cube->Render(m_Direct3D->GetDeviceContext());

    // ���� CBuffer �� 1�� ���� ������ ���
    m_Light->UseLightBuffer(m_Direct3D->GetDeviceContext(), 1);

    m_Sampler->UseLinear(m_Direct3D->GetDeviceContext(), 0);
    m_Sampler->UsePoint(m_Direct3D->GetDeviceContext(), 1);


    XMFLOAT4 gAlbedo = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    float gMetallic = 1.0f;
    float gRoughness = 0.3f;

    // �˺��� �� ���
    int gUseAlbedoMap = 1;
    int gUseOccMetalRough = 0;
    int gUseAoMap = 0;
    int gUseEmmisive = 0;
    // �븻 �� ���
    int gNormalState = 1;
    int gConvertToLinear = 0;


    // ������ ���̴��� ����Ͽ� ���� �������մϴ�.
    m_GBufferShader->Render(m_Direct3D->GetDeviceContext(), m_Cube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Cube->GetTexture(0), m_Cube->GetTexture(1), m_Cube->GetTexture(2), m_Cube->GetTexture(3), m_Cube->GetTexture(4),
        gAlbedo, gMetallic, gRoughness, gUseAlbedoMap, gUseOccMetalRough, gUseAoMap, gUseEmmisive, gNormalState, gConvertToLinear);





    // ȸ�������� ���� ����� ȸ���Ͽ� ť�갡 ȸ���ϵ��� �մϴ�.
    m_Direct3D->GetWorldMatrix(worldMatrix);


    m_Plane->GetPosition(Pos);
    worldMatrix = XMMatrixTranslation(Pos.x, Pos.y, Pos.z);


    // �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��մϴ�.
    m_Plane->Render(m_Direct3D->GetDeviceContext());

    // ���� CBuffer �� 1�� ���� ������ ���
    m_Light->UseLightBuffer(m_Direct3D->GetDeviceContext(), 1);

    m_Sampler->UseLinear(m_Direct3D->GetDeviceContext(), 0);
    m_Sampler->UsePoint(m_Direct3D->GetDeviceContext(), 1);


    gAlbedo = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    gMetallic = 0.0f;
    gRoughness = 1.0f;

    // �˺��� �� ���
    gUseAlbedoMap = 1;
    gUseOccMetalRough = 0;
    gUseAoMap = 0;
    gUseEmmisive = 0;
    // �븻 �� ���
    gNormalState = 0;
    gConvertToLinear = 0;


    // ������ ���̴��� ����Ͽ� ���� �������մϴ�.
    m_GBufferShader->Render(m_Direct3D->GetDeviceContext(), m_Plane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Plane->GetTexture(0), m_Plane->GetTexture(1), m_Plane->GetTexture(2), m_Plane->GetTexture(3), m_Plane->GetTexture(4),
        gAlbedo, gMetallic, gRoughness, gUseAlbedoMap, gUseOccMetalRough, gUseAoMap, gUseEmmisive, gNormalState, gConvertToLinear);



    // ���� ���۰� �ƴ� ������ �� ���۷� ������ Ÿ���� �ٽ� �����մϴ�.
    m_Direct3D->SetBackBufferRenderTarget();

    // ����Ʈ�� �������� �ٽ� �����մϴ�.
    m_Direct3D->ResetViewport();

    return true;
}



bool ApplicationClass::RenderDepthMap()
{
    XMMATRIX worldMatrix, lightViewProjectionMatrix;

    XMFLOAT3 Pos;

    // ������ ����� �������� �°� �����մϴ�.
    m_DepthMapTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

    // �������� �ؽ�ó�� ����ϴ�.
    m_DepthMapTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        m_DepthMapTexture->SetViewports(m_Direct3D->GetDeviceContext(), 
            m_Light->m_shadowMapProps.m_ShadowMaps[i].m_ShadowMapTopLeftX, m_Light->m_shadowMapProps.m_ShadowMaps[i].m_ShadowMapTopLeftY,
            m_Light->m_shadowMapProps.m_ShadowMaps[i].m_ShadowMapTextureRatio);

        m_Direct3D->GetWorldMatrix(worldMatrix);

        lightViewProjectionMatrix = m_Light->m_shadowMapProps.m_ShadowMaps[i].m_lightViewProjection;

        m_Cube->GetPosition(Pos);
        worldMatrix = XMMatrixTranslation(Pos.x, Pos.y, Pos.z);


        // ���� ���̴��� ť�� ���� �������մϴ�.
        m_Cube->Render(m_Direct3D->GetDeviceContext());
        bool result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Cube->GetIndexCount(), worldMatrix,
            lightViewProjectionMatrix);
        if (!result)
        {
            return false;
        }


        // ���� ����� �缳���մϴ�.
        m_Direct3D->GetWorldMatrix(worldMatrix);

        m_Plane->GetPosition(Pos);
        worldMatrix = XMMatrixTranslation(Pos.x, Pos.y, Pos.z);

        // ���� ���̴��� �׶��� ���� �������մϴ�.
        m_Plane->Render(m_Direct3D->GetDeviceContext());
        result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Plane->GetIndexCount(), worldMatrix,
            lightViewProjectionMatrix);
        if (!result)
        {
            return false;
        }
    }

    // ������ ����� ������ �� ���۷� �ٽ� �����Ѵ�.
    m_Direct3D->SetBackBufferRenderTarget();

    // ����Ʈ�� �������� �ٽ� �����մϴ�.
    m_Direct3D->ResetViewport();

    return true;
}



bool ApplicationClass::RenderEnvMap()
{
    XMVECTOR forward[6] =
    {
        { 1, 0, 0, 0 },
        { -1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, -1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, -1, 0 },
    };

    XMVECTOR up[6] =
    {
        { 0, 1, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, -1, 0 },
        { 0, 0, 1, 0 },
        { 0, 1, 0,  0 },
        { 0, 1, 0, 0 },
    };

    m_EnvironmentMap->SetViewports(m_Direct3D->GetDeviceContext());

    for (int i = 0; i < 6; ++i)
    {
        m_EnvironmentMap->SetRenderTarget(m_Direct3D->GetDeviceContext(), i);

        XMVECTOR m_EyePosition = XMVectorSet(0, 0, 0, 0);
        XMVECTOR m_LookAt = forward[i];
        XMVECTOR m_Up = up[i];

        float nearZ = 0;
        float farZ = 10;
        float viewWidth = 2;
        float viewHeight = 2;

        m_Skybox->Render(m_Direct3D->GetDeviceContext());
        m_SkyboxShader->RenderEnvMap(m_Direct3D->GetDeviceContext(), m_Skybox->GetIndexCount(),
            XMMatrixIdentity(),
            XMMatrixLookAtLH(m_EyePosition, m_LookAt, m_Up),
            XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ),
            m_SkyboxCubeMap->GetTexture());
    }


    int mapSize = 512;


    for (int i = 0; i < 6; ++i)
    {
        float roughness = (float)i / 5.0;
        m_SpecularMap->SetViewports(m_Direct3D->GetDeviceContext());

        for (int j = 0; j < 6; ++j)
        {
            m_SpecularMap->SetRenderTarget(m_Direct3D->GetDeviceContext(), i * 6 + j);


            XMVECTOR m_EyePosition = XMVectorSet(0, 0, 0, 0);
            XMVECTOR m_LookAt = forward[j];
            XMVECTOR m_Up = up[j];
            float nearZ = 0;
            float farZ = 10;
            float viewWidth = 2;
            float viewHeight = 2;

            m_Skybox->Render(m_Direct3D->GetDeviceContext());
            m_SkyboxShader->RenderEnvPreFilter(m_Direct3D->GetDeviceContext(), m_Skybox->GetIndexCount(),
                XMMatrixIdentity(),
                XMMatrixLookAtLH(m_EyePosition, m_LookAt, m_Up),
                XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ), 
                m_SkyboxCubeMap->GetTexture(), roughness);
        }

        mapSize /= 2;
    }


    m_BrdfLUT->SetRenderTarget(m_Direct3D->GetDeviceContext());

    m_Direct3D->TurnZBufferOff();

    m_Direct3D->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_SkyboxShader->RenderBrdfLUT(m_Direct3D->GetDeviceContext());

    m_Direct3D->TurnZBufferOn();

    // ���� ���۰� �ƴ� ������ �� ���۷� ������ Ÿ���� �ٽ� �����մϴ�.
    m_Direct3D->SetBackBufferRenderTarget();

    // ����Ʈ�� �������� �ٽ� �����մϴ�.
    m_Direct3D->ResetViewport();


    return true;
}



bool ApplicationClass::RenderSkybox()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    XMMATRIX scaleMatrix = XMMatrixScaling(50.0f, 50.0f, 50.0f);

    // d3d ��ü���� ���� ����� �����ɴϴ�.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);


    XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_Position->GetPosition(pos);
    worldMatrix = XMMatrixMultiply(scaleMatrix, XMMatrixTranslation(pos.x, pos.y, pos.z));


    m_Skybox->Render(m_Direct3D->GetDeviceContext());
    m_SkyboxShader->RenderSkybox(m_Direct3D->GetDeviceContext(), m_Skybox->GetIndexCount(), 
        worldMatrix, viewMatrix, projectionMatrix, m_SkyboxCubeMap->GetTexture());

    return true;
}