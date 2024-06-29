#include "Stdafx.h"
#include "InputClass.h"
#include "D3DClass.h"
#include "TimerClass.h"
#include "PositionClass.h"
#include "CameraClass.h"
#include "LightClass.h"
#include "ModelClass.h"
#include "OrthoWindowClass.h"
#include "GBuffersClass.h"
#include "GBufferShaderClass.h"
#include "DeferredShaderClass.h"


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

    // Direct3D 객체를 생성합니다.
    m_Direct3D = new D3DClass;
    if (!m_Direct3D)
    {
        return false;
    }

    // Direct3D 객체를 초기화 합니다.
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



    // 카메라 객체를 생성합니다.
    m_Camera = new CameraClass;
    if (!m_Camera)
    {
        return false;
    }

    // 카메라의 초기 위치를 설정하고 렌더링에 필요한 행렬을 만듭니다.
    m_Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -5.0f));
    m_Camera->Render();
    m_Camera->RenderBaseViewMatrix();

    // 조명 객체를 생성합니다.
    m_Light = new LightClass;
    if (!m_Light)
    {
        return false;
    }

    // 조명 객체를 초기화 합니다.
    m_Light->SetDiffuseColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
    m_Light->SetDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));



    // 모델 객체를 생성합니다.
    m_Model = new ModelClass;
    if (!m_Model)
    {
        return false;
    }


    // 그라운드 모델의 위치를 ?설정합니다.
    result = m_Model->Initialize(m_Direct3D->GetDevice(), "data/cube.txt", 1.0f);
    result = m_Model->LoadTextures(m_Direct3D->GetDevice(), L"data/seafloor.dds");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    // 전체 화면 ortho window 객체를 생성합니다.
    m_FullScreenWindow = new OrthoWindowClass;
    if (!m_FullScreenWindow)
    {
        return false;
    }

    // 전체 화면 ortho window 객체를 초기화 합니다.
    result = m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
        return false;
    }

    // 지연된 버퍼 객체를 생성합니다.
    m_GBuffers = new GBuffersClass;
    if (!m_GBuffers)
    {
        return false;
    }

    // 지연된 버퍼 객체를 초기화 합니다.
    result = m_GBuffers->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the deferred buffers object.", L"Error", MB_OK);
        return false;
    }

    // 지연된 셰이더 객체를 생성합니다.
    m_GBufferShader = new GBufferShaderClass;
    if (!m_GBufferShader)
    {
        return false;
    }

    // 지연된 셰이더 객체를 초기화 합니다.
    result = m_GBufferShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the deferred shader object.", L"Error", MB_OK);
        return false;
    }

    // 조명 쉐이더 객체를 생성합니다.
    m_DeferredShader = new DeferredShaderClass;
    if (!m_DeferredShader)
    {
        return false;
    }

    // 조명 쉐이더 객체를 초기화 합니다.
    result = m_DeferredShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
        return false;
    }

    return true;
}


void ApplicationClass::Shutdown()
{
    // 조명 쉐이더 객체를 해제합니다.
    if (m_DeferredShader)
    {
        m_DeferredShader->Shutdown();
        delete m_DeferredShader;
        m_DeferredShader = 0;
    }

    // 지연된 쉐이더 객체를 해제합니다.
    if (m_GBufferShader)
    {
        m_GBufferShader->Shutdown();
        delete m_GBufferShader;
        m_GBufferShader = 0;
    }

    // 지연된 버퍼 객체를 해제합니다.
    if (m_GBuffers)
    {
        m_GBuffers->Shutdown();
        delete m_GBuffers;
        m_GBuffers = 0;
    }

    // 전체 화면 ortho window 객체를 해제합니다.
    if (m_FullScreenWindow)
    {
        m_FullScreenWindow->Shutdown();
        delete m_FullScreenWindow;
        m_FullScreenWindow = 0;
    }

    // 모델 객체를 해제합니다.
    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
    }

    // 조명 객체를 해제합니다.
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

    // D3D 객체를 해제합니다.
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
    // 시스템의 통계를 업데이트 합니다.
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

    // 프레임 입력 처리를 수행한다.
    if (!HandleMovementInput(m_Timer->GetTime()))
    {
        return false;
    }

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

    // 카메라의 위치를 설정합니다.
    m_Camera->SetPosition(pos);
    m_Camera->SetRotation(rot);

    return true;
}



bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

    // 먼저 장면을 텍스처로 렌더링합니다.
    if (!RenderSceneToTexture())
    {
        return false;
    }

    // 장면을 시작할 버퍼를 지운다.
    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetBaseViewMatrix(baseViewMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    // 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
    m_Direct3D->TurnZBufferOff();

    // 그래픽 파이프 라인에 전체 화면 직교 윈도우 버텍스와 인덱스 버퍼를 배치하여 그리기를 준비합니다.
    m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

    // 지연 조명 쉐이더와 렌더링 버퍼를 사용하여 전체 화면 정사영 윈도우를 렌더링합니다.
    m_DeferredShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix,
        orthoMatrix, m_GBuffers->GetShaderResourceView(0), m_GBuffers->GetShaderResourceView(1),
        m_Light->GetDirection());

    // 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켭니다.
    m_Direct3D->TurnZBufferOn();

    // 렌더링 된 장면을 화면에 표시합니다.
    m_Direct3D->EndScene();

    return true;
}


bool ApplicationClass::RenderSceneToTexture()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

    // 렌더링 대상을 텍스처에 렌더링으로 설정합니다.
    m_GBuffers->SetRenderTargets(m_Direct3D->GetDeviceContext());

    // 렌더링에 텍스처를 지웁니다.
    m_GBuffers->ClearRenderTargets(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_Camera->Render();

    // d3d 객체에서 월드 행렬을 가져옵니다.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    // 각 프레임의 rotation 변수를 업데이트합니다.
    static float rotation = 0.0f;
    rotation += (float)XM_PI * 0.007f;
    if (rotation > 360.0f)
    {
        rotation -= 360.0f;
    }

    // 회전값으로 월드 행렬을 회전하여 큐브가 회전하도록 합니다.
    worldMatrix = XMMatrixRotationY(rotation);

    // 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
    m_Model->Render(m_Direct3D->GetDeviceContext());

    // 지연된 셰이더를 사용하여 모델을 렌더링합니다.
    m_GBufferShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Model->GetTexture(0));

    // 렌더 버퍼가 아닌 원래의 백 버퍼로 렌더링 타겟을 다시 설정합니다.
    m_Direct3D->SetBackBufferRenderTarget();

    // 뷰포트를 원본으로 다시 설정합니다.
    m_Direct3D->ResetViewport();

    return true;
}
