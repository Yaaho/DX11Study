#include "Stdafx.h"
#include "InputClass.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "TextureShaderClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"
#include "TextClass.h"
#include "BitmapClass.h"


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
    // 화면 너비와 높이를 저장합니다.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // 입력 개체를 만듭니다.
    m_Input = new InputClass;
    if (!m_Input)
    {
        return false;
    }

    // 입력 개체를 초기화합니다.
    bool result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
        return false;
    }

    // Direct3D 개체를 만듭니다.
    m_D3D = new D3DClass;
    if (!m_D3D)
    {
        return false;
    }

    // Direct3D 개체를 초기화합니다.
    result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    // 카메라 객체를 만듭니다.
    m_Camera = new CameraClass;
    if (!m_Camera)
    {
        return false;
    }

    // 카메라의 초기 위치를 설정합니다.
    XMMATRIX baseViewMatrix;
    m_Camera->SetPosition(XMFLOAT3(-2.0f, 0.0f, -5.0f));
    m_Camera->Render();
    m_Camera->GetViewMatrix(baseViewMatrix);

    // 모델 객체를 만듭니다.
    m_Model = new ModelClass;
    if (!m_Model)
    {
        return false;
    }

    // 모델 객체를 초기화합니다.
    result = m_Model->Initialize(m_D3D->GetDevice(), "data/sphere.txt");
    result = m_Model->LoadTextures(m_D3D->GetDevice(), L"data/blue.dds");

    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    // 텍스처 쉐이더 객체를 생성한다.
    m_TextureShader = new TextureShaderClass;
    if (!m_TextureShader)
    {
        return false;
    }

    // 텍스처 쉐이더 객체를 초기화한다.
    result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
        return false;
    }

    // 라이트 쉐이더 객체를 생성합니다.
    m_LightShader = new LightShaderClass;
    if (!m_LightShader)
    {
        return false;
    }

    // 라이트 쉐이더 객체를 초기화합니다.
    result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
        return false;
    }

    // light 객체를 만듭니다.
    m_Light = new LightClass;
    if (!m_Light)
    {
        return false;
    }

    // 라이트 오브젝트를 초기화한다.
    m_Light->SetDirection(0.0f, 0.0f, 1.0f);

    // 텍스트 객체를 만듭니다.
    m_Text = new TextClass;
    if (!m_Text)
    {
        return false;
    }

    // 텍스트 객체를 초기화합니다.
    result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight,
        baseViewMatrix);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
        return false;
    }

    // 비트맵 객체를 만듭니다.
    m_Bitmap = new BitmapClass;
    if (!m_Bitmap)
    {
        return false;
    }

    // 비트맵 객체를 초기화합니다.
    result = m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"data/mouse.dds", 32, 32);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
        return false;
    }



    return true;
}


void ApplicationClass::Shutdown()
{

    // 비트맵 객체를 해제합니다.
    if (m_Bitmap)
    {
        m_Bitmap->Shutdown();
        delete m_Bitmap;
        m_Bitmap = 0;
    }

    // 텍스트 객체를 해제합니다.
    if (m_Text)
    {
        m_Text->Shutdown();
        delete m_Text;
        m_Text = 0;
    }

    // light 객체를 해제합니다.
    if (m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    // 라이트 쉐이더 객체를 해제합니다.
    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
    }

    // 텍스처 쉐이더 객체를 해제한다.
    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = 0;
    }

    // 모델 객체를 해제합니다.
    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
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

    // 입력 객체를 해제합니다.
    if (m_Input)
    {
        m_Input->Shutdown();
        delete m_Input;
        m_Input = 0;
    }
}

bool ApplicationClass::Frame(float frameTime)
{
    bool result = HandleInput();
    if (!result)
    {
        return false;
    }

    result = Render();
    if (!result)
    {
        return false;
    }


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


    return true;
}

bool ApplicationClass::HandleInput()
{
    int mouseX = 0;
    int mouseY = 0;
    
    // 입력 프레임 처리를 수행한다.
    if (m_Input->Frame() == false)
    {
        return false;
    }

    // 사용자가 이스케이프를 눌렀을 때 응용 프로그램을 종료할 것인지 확인한다.
    if (m_Input->IsEscapePressed() == true)
    {
        return false;
    }

    // 마우스 왼쪽 버튼이 눌러졌는지 확인한다.
    if (m_Input->IsLeftMouseButtonDown() == true)
    {
        // 마우스로 화면을 클릭한 다음 교체 테스트를 수행한다.
        if (m_beginCheck == false)
        {
            m_beginCheck = true;
            m_Input->GetMouseLocation(mouseX, mouseY);
            TestIntersection(mouseX, mouseY);
        }
    }

    if (m_Input->IsLeftMouseButtonDown() == false)
    {
        m_beginCheck = false;
    }


    return true;
}


bool ApplicationClass::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, translateMatrix;

    // 장면을 시작할 버퍼를 지운다.
    m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // 카메라의 위치에 따라 뷰 행렬을 생성합니다.
    m_Camera->Render();

    // 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
    m_Camera->GetViewMatrix(viewMatrix);
    m_D3D->GetWorldMatrix(worldMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);
    m_D3D->GetOrthoMatrix(orthoMatrix);

    // 구의 위치로 변환합니다.
    translateMatrix = XMMatrixTranslation(-5.0f, 1.0f, 5.0f);
    worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

    // 라이트 쉐이더를 사용하여 모델을 렌더링합니다.
    m_Model->Render(m_D3D->GetDeviceContext());
    if (m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Model->GetTexture(0), m_Light->GetDirection()) == false)
    {
        return false;
    }

    // 월드 행렬을 재설정합니다.
    m_D3D->GetWorldMatrix(worldMatrix);

    // 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
    m_D3D->TurnZBufferOff();

    // 알파 블렌딩을 켭니다.
    m_D3D->EnableAlphaBlending();

    // 입력 객체에서 마우스의 위치를 ??가져옵니다.
    int mouseX = 0;
    int mouseY = 0;
    m_Input->GetMouseLocation(mouseX, mouseY);

    // 텍스처 셰이더로 마우스 커서를 렌더링합니다.
    if (m_Bitmap->Render(m_D3D->GetDeviceContext(), mouseX, mouseY) == false)
    {
        return false;
    }

    if (m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
        m_Bitmap->GetTexture()) == false)
    {
        return false;
    }

    // 텍스트 문자열을 렌더링합니다.
    if (m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix) == false)
    {
        return false;
    }

    // 알파 블렌딩을 돌립니다.
    m_D3D->DisableAlphaBlending();

    // 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
    m_D3D->TurnZBufferOn();

    // 렌더링 된 장면을 화면에 표시합니다.
    m_D3D->EndScene();

    return true;
}


void ApplicationClass::TestIntersection(int mouseX, int mouseY)
{
    XMMATRIX projectionMatrix, viewMatrix, inverseViewMatrix, worldMatrix, translateMatrix, inverseWorldMatrix;
    XMFLOAT3 direction, origin, rayOrigin, rayDirection;

    // 마우스 커서 좌표를 -1에서 +1 범위로 이동합니다
    float pointX = ((2.0f * (float)mouseX) / (float)m_screenWidth) - 1.0f;
    float pointY = (((2.0f * (float)mouseY) / (float)m_screenHeight) - 1.0f) * -1.0f;

    // 뷰포트의 종횡비를 고려하여 투영 행렬을 사용하여 점을 조정한다.
    m_D3D->GetProjectionMatrix(projectionMatrix);

    XMFLOAT3X3 projectionMatrix4;
    XMStoreFloat3x3(&projectionMatrix4, projectionMatrix);

    pointX = pointX / projectionMatrix4._11;
    pointY = pointY / projectionMatrix4._22;

    // 뷰 행렬의 역함수를 구한다.
    m_Camera->GetViewMatrix(viewMatrix);
    inverseViewMatrix = XMMatrixInverse(nullptr, viewMatrix);

    XMFLOAT3X3 inverseViewMatrix4;
    XMStoreFloat3x3(&inverseViewMatrix4, inverseViewMatrix);

    // 뷰 공간에서 피킹 레이의 방향을 계산합니다.
    direction.x = (pointX * inverseViewMatrix4._11) + (pointY * inverseViewMatrix4._21) + inverseViewMatrix4._31;
    direction.y = (pointX * inverseViewMatrix4._12) + (pointY * inverseViewMatrix4._22) + inverseViewMatrix4._32;
    direction.z = (pointX * inverseViewMatrix4._13) + (pointY * inverseViewMatrix4._23) + inverseViewMatrix4._33;

    origin = m_Camera->GetPosition();

    // 세계 행렬을 가져와 구의 위치로 변환한다.
    m_D3D->GetWorldMatrix(worldMatrix);
    translateMatrix = XMMatrixTranslation(-5.0f, 1.0f, 5.0f);
    worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);


    // Translation 행렬의 역함수를 구한다.
    inverseWorldMatrix = XMMatrixInverse(nullptr, worldMatrix);


    // 이제 광선 원점과 광선 방향을 뷰 공간에서 월드 공간으로 변환합니다.
    XMStoreFloat3(&rayOrigin, XMVector3TransformCoord(XMVectorSet(origin.x, origin.y, origin.z, 0.0f), inverseWorldMatrix));
    XMStoreFloat3(&direction, XMVector3TransformNormal(XMVectorSet(direction.x, direction.y, direction.z, 0.0f),
        inverseWorldMatrix));

    // 광선 방향을 표준화합니다.
    XMStoreFloat3(&rayDirection, XMVector3Normalize(XMVectorSet(direction.x, direction.y, direction.z, 0.0f)));

    // 이제 광선 구 교차 테스트를 수행하십시오.
    if (RaySphereIntersect(rayOrigin, rayDirection, 1.0f) == true)
    {
        // 교차하는 경우 화면에 표시되는 텍스트 문자열에서 교차로를 "yes"로 설정합니다.
        m_Text->SetIntersection(true, m_D3D->GetDeviceContext());
    }
    else
    {
        // 그렇지 않으면 "No"로 교차를 설정하십시오.
        m_Text->SetIntersection(false, m_D3D->GetDeviceContext());
    }
}


bool ApplicationClass::RaySphereIntersect(XMFLOAT3 rayOrigin, XMFLOAT3 rayDirection, float radius)
{
    // a, b 및 c 계수를 계산합니다.
    float a = (rayDirection.x * rayDirection.x) + (rayDirection.y * rayDirection.y) + (rayDirection.z * rayDirection.z);
    float b = ((rayDirection.x * rayOrigin.x) + (rayDirection.y * rayOrigin.y) + (rayDirection.z * rayOrigin.z)) * 2.0f;
    float c = ((rayOrigin.x * rayOrigin.x) + (rayOrigin.y * rayOrigin.y) + (rayOrigin.z * rayOrigin.z)) - (radius * radius);

    // 결과값을 얻는다
    float discriminant = (b * b) - (4 * a * c);

    // 결과값이 음수이면 피킹 선이 구를 벗어난 것입니다. 그렇지 않으면 구를 선택합니다.
    if (discriminant < 0.0f)
    {
        return false;
    }

    return true;
}
