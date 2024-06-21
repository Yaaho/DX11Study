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
    // Direct3D 객체 생성
    m_D3D = new D3DClass;
    if (!m_D3D)
    {
        return false;
    }

    // Direct3D 객체 초기화
    bool result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
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

    // 카메라 포지션을 설정한다
    m_Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -10.0f));

    // 큐브 모델 오브젝트를 생성합니다.
    m_CubeModel = new ModelClass;
    if (!m_CubeModel)
    {
        return false;
    }

    // 큐브 모델 오브젝트를 초기화 합니다.
    result = m_CubeModel->Initialize(m_D3D->GetDevice(), "data/cube.txt");
    result = m_CubeModel->LoadTextures(m_D3D->GetDevice(), L"data/wall01.dds");

    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the cube model object.", L"Error", MB_OK);
        return false;
    }

    // 큐브 모델의 위치를 ??설정 합니다.
    m_CubeModel->SetPosition(-2.0f, 2.0f, 0.0f);

    // 구형 모델 객체를 만듭니다.
    m_SphereModel = new ModelClass;
    if (!m_SphereModel)
    {
        return false;
    }

    // 구형 모델 객체를 초기화합니다.
    result = m_SphereModel->Initialize(m_D3D->GetDevice(), "data/sphere.txt");
    result = m_SphereModel->LoadTextures(m_D3D->GetDevice(), L"data/ice.dds");

    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK);
        return false;
    }

    // 구형 모델의 위치를 ??설정합니다.
    m_SphereModel->SetPosition(2.0f, 2.0f, 0.0f);

    // 지면 모델 객체를 만듭니다.
    m_GroundModel = new ModelClass;
    if (!m_GroundModel)
    {
        return false;
    }

    // 지면 모델 객체를 초기화합니다.
    result = m_GroundModel->Initialize(m_D3D->GetDevice(), "data/plane01.txt");
    result = m_GroundModel->LoadTextures(m_D3D->GetDevice(), L"data/metal001.dds");

    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
        return false;
    }

    // 지면 모델의 위치를 ??설정합니다.
    m_GroundModel->SetPosition(0.0f, 1.0f, 0.0f);

    // light 객체를 만듭니다.
    m_Light = new LightClass;
    if (!m_Light)
    {
        return false;
    }

    // 조명 객체를 초기화합니다.
    m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->GenerateOrthoMatrix(20.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);

    // 렌더링을 텍스처 오브젝트에 생성한다.
    m_RenderTexture = new RenderTextureClass;
    if (!m_RenderTexture)
    {
        return false;
    }

    // 렌더링을 텍스처 오브젝트를 초기화한다.
    result = m_RenderTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT,
        SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
        return false;
    }

    // 깊이 셰이더 개체를 만듭니다.
    m_DepthShader = new DepthShaderClass;
    if (!m_DepthShader)
    {
        return false;
    }

    // 깊이 셰이더 개체를 초기화합니다.
    result = m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
        return false;
    }

    // 그림자 셰이더 개체를 만듭니다.
    m_ShadowShader = new ShadowShaderClass;
    if (!m_ShadowShader)
    {
        return false;
    }

    // 그림자 쉐이더 객체를 초기화합니다.
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
    // 그림자 쉐이더 객체를 해제합니다.
    if (m_ShadowShader)
    {
        m_ShadowShader->Shutdown();
        delete m_ShadowShader;
        m_ShadowShader = 0;
    }

    // 깊이 셰이더 개체를 해제합니다.
    if (m_DepthShader)
    {
        m_DepthShader->Shutdown();
        delete m_DepthShader;
        m_DepthShader = 0;
    }

    // 렌더 투 텍스쳐 객체를 해제합니다.
    if (m_RenderTexture)
    {
        m_RenderTexture->Shutdown();
        delete m_RenderTexture;
        m_RenderTexture = 0;
    }

    // 조명 객체를 해제합니다.
    if (m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    // 지면 모델 객체를 해제합니다.
    if (m_GroundModel)
    {
        m_GroundModel->Shutdown();
        delete m_GroundModel;
        m_GroundModel = 0;
    }

    // 구형 모델 객체를 해제합니다.
    if (m_SphereModel)
    {
        m_SphereModel->Shutdown();
        delete m_SphereModel;
        m_SphereModel = 0;
    }

    // 큐브 모델 객체를 해제합니다.
    if (m_CubeModel)
    {
        m_CubeModel->Shutdown();
        delete m_CubeModel;
        m_CubeModel = 0;
    }

    // 카메라 객체를 해제합니다.
    if (m_Camera)
    {
        delete m_Camera;
        m_Camera = 0;
    }

    // D3D 객체를 해제합니다.
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
 
    // 각 프레임의 조명 위치를 업데이트 합니다.
    lightPosX -= 0.003f * frameTime;
 
    // 각 프레임의 조명 각도를 업데이트 합니다.
    lightAngle -= 0.03f * frameTime;
    if(lightAngle < 90.0f)
    {
        lightAngle = 270.0f;
 
        // 조명 위치도 재설정 합니다.
        lightPosX = 9.0f;
    }
    float radians = lightAngle * 0.0174532925f;
 
    // 빛의 방향을 업데이 트합니다.
    m_Light->SetDirection(sinf(radians), cosf(radians), 0.0f);
 
    // 빛의 위치와 표시를 설정합니다.
    m_Light->SetPosition(lightPosX, 8.0f, -0.1f);
    m_Light->SetLookAt(-lightPosX, 0.0f, 0.0f);
    
    // 그래픽 장면을 렌더링합니다.
    return Render();


    /*
    // 카메라 위치를 설정합니다.
    m_Camera->SetPosition(XMFLOAT3(posX, posY, posZ));
    m_Camera->SetRotation(XMFLOAT3(rotX, rotY, rotZ));
    */


    /*
    초당 프레임 수를 설정한다.
    if (!m_Text->SetFps(fps, m_Direct3D->GetDeviceContext()))
    {
        return false;
    }

    cpu 사용을 설정한다.
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

    // 렌더링 대상을 렌더링에 맞게 설정합니다.
    m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext());

    // 렌더링을 텍스처에 지웁니다.
    m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    // 조명의 위치에 따라 조명보기 행렬을 생성합니다.
    m_Light->GenerateViewMatrix();

    // d3d 객체에서 세계 행렬을 가져옵니다.
    m_D3D->GetWorldMatrix(worldMatrix);

    // 라이트 오브젝트에서 뷰 및 정사각형 매트릭스를 가져옵니다.
    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    // 큐브 모델에 대한 변환 행렬을 설정하십시오.
    m_CubeModel->GetPosition(posX, posY, posZ);
    worldMatrix = XMMatrixTranslation(posX, posY, posZ);

    // 깊이 셰이더로 큐브 모델을 렌더링합니다.
    m_CubeModel->Render(m_D3D->GetDeviceContext());
    bool result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix,
        lightViewMatrix, lightOrthoMatrix);
    if (!result)
    {
        return false;
    }

    // 월드 행렬을 재설정합니다.
    m_D3D->GetWorldMatrix(worldMatrix);

    // 구형 모델에 대한 변환 행렬을 설정합니다.
    m_SphereModel->GetPosition(posX, posY, posZ);
    worldMatrix = XMMatrixTranslation(posX, posY, posZ);

    // 깊이 셰이더로 구형 모델을 렌더링합니다.
    m_SphereModel->Render(m_D3D->GetDeviceContext());
    result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix,
        lightViewMatrix, lightOrthoMatrix);
    if (!result)
    {
        return false;
    }

    // 월드 행렬을 재설정합니다.
    m_D3D->GetWorldMatrix(worldMatrix);

    // ground 모델에 대한 변환 행렬을 설정합니다.
    m_GroundModel->GetPosition(posX, posY, posZ);
    worldMatrix = XMMatrixTranslation(posX, posY, posZ);

    // 깊이 셰이더로 그라운드 모델을 렌더링합니다.
    m_GroundModel->Render(m_D3D->GetDeviceContext());
    result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix,
        lightViewMatrix, lightOrthoMatrix);
    if (!result)
    {
        return false;
    }

    // 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
    m_D3D->SetBackBufferRenderTarget();

    // 뷰포트를 원본으로 다시 설정합니다.
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

    // 먼저 장면을 텍스처로 렌더링합니다.
    bool result = RenderSceneToTexture();
    if (!result)
    {
        return false;
    }

    // 장면을 시작할 버퍼를 지운다.
    m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // 카메라의 위치에 따라 뷰 행렬을 생성합니다.
    m_Camera->Render();

    // 조명의 위치에 따라 조명보기 행렬을 생성합니다.
    m_Light->GenerateViewMatrix();

    // 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
    m_Camera->GetViewMatrix(viewMatrix);
    m_D3D->GetWorldMatrix(worldMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);

    // 라이트 오브젝트로부터 라이트의 뷰와 투영 행렬을 가져옵니다.
    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    // 큐브 모델에 대한 변환 행렬을 설정하십시오.
    m_CubeModel->GetPosition(posX, posY, posZ);
    worldMatrix = XMMatrixTranslation(posX, posY, posZ);

    // 큐브 모델 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
    m_CubeModel->Render(m_D3D->GetDeviceContext());

    // 그림자 쉐이더를 사용하여 모델을 렌더링합니다.
    result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_CubeModel->GetTexture(0),
        m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(),
        m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

    // 월드 행렬을 재설정합니다.
    m_D3D->GetWorldMatrix(worldMatrix);

    // 구형 모델에 대한 변환 행렬을 설정합니다.
    m_SphereModel->GetPosition(posX, posY, posZ);
    worldMatrix = XMMatrixTranslation(posX, posY, posZ);

    // 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
    m_SphereModel->Render(m_D3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_SphereModel->GetIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_SphereModel->GetTexture(0),
        m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(),
        m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

    // 월드 행렬을 재설정합니다.
    m_D3D->GetWorldMatrix(worldMatrix);

    // ground 모델에 대한 변환 행렬을 설정합니다.
    m_GroundModel->GetPosition(posX, posY, posZ);
    worldMatrix = XMMatrixTranslation(posX, posY, posZ);

    // 그림자 쉐이더를 사용하여 그라운드 모델을 렌더링합니다.
    m_GroundModel->Render(m_D3D->GetDeviceContext());
    result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_GroundModel->GetTexture(0),
        m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(),
        m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
    if (!result)
    {
        return false;
    }

    // 렌더링 된 장면을 화면에 표시합니다.
    m_D3D->EndScene();

    return true;
}


