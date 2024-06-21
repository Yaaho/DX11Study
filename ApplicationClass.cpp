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


    // 입력 개체를 만듭니다. 입력 객체는 사용자로부터 키보드 및 마우스 입력 읽기를 처리하는 데 사용됩니다.
    m_Input = new InputClass;
    if (!m_Input)
    {
        return false;
    }

    // 입력 개체를 초기화 합니다.
    result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
        return false;
    }

    // Direct3D 개체를 만듭니다.
    m_Direct3D = new D3DClass;
    if (!m_Direct3D)
    {
        return false;
    }

    // Direct3D 개체를 초기화 합니다.
    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
        return false;
    }

    // 타이머 개체를 만듭니다.
    m_Timer = new TimerClass;
    if (!m_Timer)
    {
        return false;
    }

    // 타이머 개체를 초기화 합니다.
    result = m_Timer->Initialize();
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
        return false;
    }

    // 위치 개체를 만듭니다.
    m_Position = new PositionClass;
    if (!m_Position)
    {
        return false;
    }

    // 초기 위치를 설정합니다.
    m_Position->SetPosition(0.0f, 7.0f, -11.0f);
    m_Position->SetRotation(20.0f, 0.0f, 0.0f);

    // 카메라 객체를 만듭니다.
    m_Camera = new CameraClass;
    if (!m_Camera)
    {
        return false;
    }

    // 광원 개체를 만듭니다.
    m_Light = new LightClass;
    if (!m_Light)
    {
        return false;
    }

    // 광원 객체를 초기화 합니다.
    m_Light->GenerateOrthoMatrix(15.0f, 15.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);

    // 그라운드 모델 객체를 만듭니다.
    m_GroundModel = new ModelClass;
    if (!m_GroundModel)
    {
        return false;
    }

    // 그라운드 모델의 위치를 ?설정합니다.
    result = m_GroundModel->Initialize(m_Direct3D->GetDevice(), "data/plane01.txt", 2.0f);
    result = m_GroundModel->LoadTextures(m_Direct3D->GetDevice(), L"data/dirt.dds");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
        return false;
    }

    // 지면 모델 배치 위치를 설정합니다.
    m_GroundModel->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));

    // 나무 객체를 만듭니다.
    m_Tree = new TreeClass;
    if (!m_Tree)
    {
        return false;
    }

    // 그림자 셰이더 개체를 초기화합니다.
    result = m_Tree->Initialize(m_Direct3D->GetDevice(), "data/trees/trunk001.txt",
        L"data/trees/trunk001.dds", "data/trees/leaf001.txt",
        L"data/trees/leaf001.dds", 0.1f);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the tree object.", L"Error", MB_OK);
        return false;
    }

    // 나무 모델의 위치를 ??설정합니다.
    m_Tree->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));

    // 텍스처 객체에 렌더링을 만듭니다.
    m_RenderTexture = new RenderTextureClass;
    if (!m_RenderTexture)
    {
        return false;
    }

    // 렌더링을 텍스처 객체로 초기화합니다.
    result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT,
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
    result = m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
        return false;
    }

    // 투명한 깊이 셰이더 개체를 만듭니다.
    m_TransparentDepthShader = new TransparentDepthShaderClass;
    if (!m_TransparentDepthShader)
    {
        return false;
    }

    // 투명한 깊이 셰이더 개체를 초기화합니다.
    result = m_TransparentDepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the transparent depth shader object.", L"Error", MB_OK);
        return false;
    }

    // 그림자 셰이더 개체를 만듭니다.
    m_ShadowShader = new ShadowShaderClass;
    if (!m_ShadowShader)
    {
        return false;
    }

    // 그림자 쉐이더 객체를 초기화합니다.
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
    // 그림자 쉐이더 객체를 해제합니다.
    if (m_ShadowShader)
    {
        m_ShadowShader->Shutdown();
        delete m_ShadowShader;
        m_ShadowShader = 0;
    }

    // 투명한 깊이 셰이더 개체를 해제합니다.
    if (m_TransparentDepthShader)
    {
        m_TransparentDepthShader->Shutdown();
        delete m_TransparentDepthShader;
        m_TransparentDepthShader = 0;
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

    // 나무 객체를 해제합니다.
    if (m_Tree)
    {
        m_Tree->Shutdown();
        delete m_Tree;
        m_Tree = 0;
    }

    // 지면 모델 객체를 해제합니다.
    if (m_GroundModel)
    {
        m_GroundModel->Shutdown();
        delete m_GroundModel;
        m_GroundModel = 0;
    }

    // 광원 객체를 해제합니다.
    if (m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    // 카메라 객체를 해제합니다.
    if (m_Camera)
    {
        delete m_Camera;
        m_Camera = 0;
    }

    // 위치 개체를 해제합니다.
    if (m_Position)
    {
        delete m_Position;
        m_Position = 0;
    }

    // 타이머 개체를 해제합니다.
    if (m_Timer)
    {
        delete m_Timer;
        m_Timer = 0;
    }

    // Direct3D 개체를 해제합니다.
    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = 0;
    }

    // 입력 개체를 해제합니다.
    if (m_Input)
    {
        m_Input->Shutdown();
        delete m_Input;
        m_Input = 0;
    }
}

bool ApplicationClass::Frame()
{
    // 시스템 통계를 업데이트 합니다.
    m_Timer->Frame();

    // 사용자 입력을 읽습니다.
    if (!m_Input->Frame())
    {
        return false;
    }

    // 사용자가 ESC 키를 누르고 응용 프로그램을 종료할 것인지 확인합니다.
    if (m_Input->IsEscapePressed() == true)
    {
        return false;
    }

    // 프레임 입력 처리를 수행합니다.
    if (!HandleMovementInput(m_Timer->GetTime()))
    {
        return false;
    }

    // 장면의 조명을 업데이트 합니다.
    UpdateLighting();

    // 그래픽을 렌더링 합니다.
    return Render();

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


bool ApplicationClass::HandleMovementInput(float frameTime)
{
    XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3 rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

    // 갱신된 위치를 계산하기 위한 프레임 시간을 설정합니다.
    m_Position->SetFrameTime(frameTime);

    // 입력을 처리합니다.
    m_Position->TurnLeft(m_Input->IsLeftPressed());
    m_Position->TurnRight(m_Input->IsRightPressed());
    m_Position->MoveForward(m_Input->IsUpPressed());
    m_Position->MoveBackward(m_Input->IsDownPressed());
    m_Position->MoveUpward(m_Input->IsAPressed());
    m_Position->MoveDownward(m_Input->IsZPressed());
    m_Position->LookUpward(m_Input->IsPgUpPressed());
    m_Position->LookDownward(m_Input->IsPgDownPressed());

    // 시점 위치 / 회전을 가져옵니다.
    m_Position->GetPosition(pos);
    m_Position->GetRotation(rot);

    // 카메라의 위치를 ??설정합니다.
    m_Camera->SetPosition(pos);
    m_Camera->SetRotation(rot);

    return true;
}


void ApplicationClass::UpdateLighting()
{
    static float angle = 270.0f;
    static float offsetX = 9.0f;

    // 빛의 방향을 업데이트 합니다.
    angle -= 0.03f * m_Timer->GetTime();
    if (angle < 90.0f)
    {
        angle = 270.0f;
        offsetX = 9.0f;
    }
    float radians = angle * 0.0174532925f;
    m_Light->SetDirection(XMFLOAT3(sinf(radians), cosf(radians), 0.0f));

    // 조회 및 위치를 업데이트 합니다.
    offsetX -= 0.003f * m_Timer->GetTime();
    m_Light->SetPosition(XMFLOAT3(0.0f + offsetX, 10.0f, 1.0f));
    m_Light->SetLookAt(XMFLOAT3(0.0f - offsetX, 0.0f, 2.0f));
}


bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightOrthoMatrix;
    XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);

    // 먼저 장면을 텍스처로 렌더링합니다.
    if (!RenderSceneToTexture())
    {
        return false;
    }

    // 장면을 시작할 버퍼를 지운다.
    m_Direct3D->BeginScene(0.0f, 0.5f, 0.8f, 1.0f);

    // 카메라의 위치에 따라 뷰 행렬을 생성합니다.
    m_Camera->Render();

    // 조명의 위치에 따라 조명보기 행렬을 생성합니다.
    m_Light->GenerateViewMatrix();

    // 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    // 라이트 오브젝트로부터 라이트의 뷰와 투영 행렬을 가져옵니다.
    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    // 밝은 색상 속성을 설정합니다.
    XMFLOAT4 diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    XMFLOAT4 ambientColor = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

    // 지상 모델의 위치로 변환합니다.
    m_GroundModel->GetPosition(pos);
    worldMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

    // 그림자 쉐이더를 사용하여 그라운드 모델을 렌더링 합니다.
    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_GroundModel->GetTexture(0),
        m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(),
        ambientColor, diffuseColor);
    m_Direct3D->GetWorldMatrix(worldMatrix);

    // 트리 모델의 위치로 변환합니다.
    m_Tree->GetPosition(pos);
    worldMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

    // 나무 줄기를 렌더링 합니다.
    m_Tree->RenderTrunk(m_Direct3D->GetDeviceContext());
    m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Tree->GetTrunkIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_Tree->GetTrunkTexture(),
        m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(),
        ambientColor, diffuseColor);

    // 블렌딩을 활성화하고 나무를 렌더링 합니다.
    m_Direct3D->EnableAlphaBlending();
    m_Tree->RenderLeaves(m_Direct3D->GetDeviceContext());
    m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Tree->GetLeafIndexCount(), worldMatrix, viewMatrix,
        projectionMatrix, lightViewMatrix, lightOrthoMatrix, m_Tree->GetLeafTexture(),
        m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(), ambientColor,
        diffuseColor);

    m_Direct3D->DisableAlphaBlending();
    m_Direct3D->GetWorldMatrix(worldMatrix);

    // 렌더링 된 장면을 화면에 출력합니다.
    m_Direct3D->EndScene();

    return true;
}


bool ApplicationClass::RenderSceneToTexture()
{
    XMMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix;
    XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);

    // 렌더링 대상을 텍스처에 렌더링으로 설정합니다.
    m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

    // 렌더링에 텍스처를 지웁니다.
    m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    // d3d 객체에서 월드 행렬을 가져옵니다.
    m_Direct3D->GetWorldMatrix(worldMatrix);

    // 조명의 위치에 따라 조명보기 행렬을 생성합니다.
    m_Light->GenerateViewMatrix();

    // 라이트 오브젝트에서 뷰 및 정사각형 매트릭스를 가져옵니다.
    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    // 나무의 위치로 변환합니다.
    m_Tree->GetPosition(pos);
    worldMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

    // 깊이 쉐이더로 나무 트렁크를 렌더링합니다.
    m_Tree->RenderTrunk(m_Direct3D->GetDeviceContext());
    m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Tree->GetTrunkIndexCount(), worldMatrix, lightViewMatrix,
        lightOrthoMatrix);

    // 깊이 투명도 셰이더를 사용하여 나무와 나뭇잎을 렌더링 합니다.
    m_Tree->RenderLeaves(m_Direct3D->GetDeviceContext());
    if (!m_TransparentDepthShader->Render(m_Direct3D->GetDeviceContext(), m_Tree->GetLeafIndexCount(), worldMatrix,
        lightViewMatrix, lightOrthoMatrix, m_Tree->GetLeafTexture()))
    {
        return false;
    }

    // 월드 행렬을 재설정합니다.
    m_Direct3D->GetWorldMatrix(worldMatrix);

    // ground 모델에 대한 변환 행렬을 설정합니다.
    m_GroundModel->GetPosition(pos);
    worldMatrix = XMMatrixTranslation(pos.x, pos.y, pos.z);

    // 그림자 쉐이더를 사용하여 그라운드 모델을 렌더링합니다.
    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, lightViewMatrix,
        lightOrthoMatrix);

    // 렌더링 대상을 원래의 백 버퍼로 다시 설정하고 렌더링에 대한 렌더링을 더 이상 다시 설정하지 않습니다.
    m_Direct3D->SetBackBufferRenderTarget();

    // 뷰포트를 원본으로 다시 설정합니다.
    m_Direct3D->ResetViewport();

    return true;
}
