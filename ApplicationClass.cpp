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
    // ȭ�� �ʺ�� ���̸� �����մϴ�.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // �Է� ��ü�� ����ϴ�.
    m_Input = new InputClass;
    if (!m_Input)
    {
        return false;
    }

    // �Է� ��ü�� �ʱ�ȭ�մϴ�.
    bool result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
        return false;
    }

    // Direct3D ��ü�� ����ϴ�.
    m_D3D = new D3DClass;
    if (!m_D3D)
    {
        return false;
    }

    // Direct3D ��ü�� �ʱ�ȭ�մϴ�.
    result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    // ī�޶� ��ü�� ����ϴ�.
    m_Camera = new CameraClass;
    if (!m_Camera)
    {
        return false;
    }

    // ī�޶��� �ʱ� ��ġ�� �����մϴ�.
    XMMATRIX baseViewMatrix;
    m_Camera->SetPosition(XMFLOAT3(-2.0f, 0.0f, -5.0f));
    m_Camera->Render();
    m_Camera->GetViewMatrix(baseViewMatrix);

    // �� ��ü�� ����ϴ�.
    m_Model = new ModelClass;
    if (!m_Model)
    {
        return false;
    }

    // �� ��ü�� �ʱ�ȭ�մϴ�.
    result = m_Model->Initialize(m_D3D->GetDevice(), "data/sphere.txt");
    result = m_Model->LoadTextures(m_D3D->GetDevice(), L"data/blue.dds");

    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    // �ؽ�ó ���̴� ��ü�� �����Ѵ�.
    m_TextureShader = new TextureShaderClass;
    if (!m_TextureShader)
    {
        return false;
    }

    // �ؽ�ó ���̴� ��ü�� �ʱ�ȭ�Ѵ�.
    result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
        return false;
    }

    // ����Ʈ ���̴� ��ü�� �����մϴ�.
    m_LightShader = new LightShaderClass;
    if (!m_LightShader)
    {
        return false;
    }

    // ����Ʈ ���̴� ��ü�� �ʱ�ȭ�մϴ�.
    result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
        return false;
    }

    // light ��ü�� ����ϴ�.
    m_Light = new LightClass;
    if (!m_Light)
    {
        return false;
    }

    // ����Ʈ ������Ʈ�� �ʱ�ȭ�Ѵ�.
    m_Light->SetDirection(0.0f, 0.0f, 1.0f);

    // �ؽ�Ʈ ��ü�� ����ϴ�.
    m_Text = new TextClass;
    if (!m_Text)
    {
        return false;
    }

    // �ؽ�Ʈ ��ü�� �ʱ�ȭ�մϴ�.
    result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight,
        baseViewMatrix);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
        return false;
    }

    // ��Ʈ�� ��ü�� ����ϴ�.
    m_Bitmap = new BitmapClass;
    if (!m_Bitmap)
    {
        return false;
    }

    // ��Ʈ�� ��ü�� �ʱ�ȭ�մϴ�.
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

    // ��Ʈ�� ��ü�� �����մϴ�.
    if (m_Bitmap)
    {
        m_Bitmap->Shutdown();
        delete m_Bitmap;
        m_Bitmap = 0;
    }

    // �ؽ�Ʈ ��ü�� �����մϴ�.
    if (m_Text)
    {
        m_Text->Shutdown();
        delete m_Text;
        m_Text = 0;
    }

    // light ��ü�� �����մϴ�.
    if (m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    // ����Ʈ ���̴� ��ü�� �����մϴ�.
    if (m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
    }

    // �ؽ�ó ���̴� ��ü�� �����Ѵ�.
    if (m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = 0;
    }

    // �� ��ü�� �����մϴ�.
    if (m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
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


    return true;
}

bool ApplicationClass::HandleInput()
{
    int mouseX = 0;
    int mouseY = 0;
    
    // �Է� ������ ó���� �����Ѵ�.
    if (m_Input->Frame() == false)
    {
        return false;
    }

    // ����ڰ� �̽��������� ������ �� ���� ���α׷��� ������ ������ Ȯ���Ѵ�.
    if (m_Input->IsEscapePressed() == true)
    {
        return false;
    }

    // ���콺 ���� ��ư�� ���������� Ȯ���Ѵ�.
    if (m_Input->IsLeftMouseButtonDown() == true)
    {
        // ���콺�� ȭ���� Ŭ���� ���� ��ü �׽�Ʈ�� �����Ѵ�.
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

    // ����� ������ ���۸� �����.
    m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // ī�޶��� ��ġ�� ���� �� ����� �����մϴ�.
    m_Camera->Render();

    // ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����ɴϴ�.
    m_Camera->GetViewMatrix(viewMatrix);
    m_D3D->GetWorldMatrix(worldMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);
    m_D3D->GetOrthoMatrix(orthoMatrix);

    // ���� ��ġ�� ��ȯ�մϴ�.
    translateMatrix = XMMatrixTranslation(-5.0f, 1.0f, 5.0f);
    worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

    // ����Ʈ ���̴��� ����Ͽ� ���� �������մϴ�.
    m_Model->Render(m_D3D->GetDeviceContext());
    if (m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Model->GetTexture(0), m_Light->GetDirection()) == false)
    {
        return false;
    }

    // ���� ����� �缳���մϴ�.
    m_D3D->GetWorldMatrix(worldMatrix);

    // ��� 2D �������� �����Ϸ��� Z ���۸� ���ϴ�.
    m_D3D->TurnZBufferOff();

    // ���� ������ �մϴ�.
    m_D3D->EnableAlphaBlending();

    // �Է� ��ü���� ���콺�� ��ġ�� ??�����ɴϴ�.
    int mouseX = 0;
    int mouseY = 0;
    m_Input->GetMouseLocation(mouseX, mouseY);

    // �ؽ�ó ���̴��� ���콺 Ŀ���� �������մϴ�.
    if (m_Bitmap->Render(m_D3D->GetDeviceContext(), mouseX, mouseY) == false)
    {
        return false;
    }

    if (m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
        m_Bitmap->GetTexture()) == false)
    {
        return false;
    }

    // �ؽ�Ʈ ���ڿ��� �������մϴ�.
    if (m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix) == false)
    {
        return false;
    }

    // ���� ������ �����ϴ�.
    m_D3D->DisableAlphaBlending();

    // ��� 2D �������� �Ϸ�Ǿ����Ƿ� Z ���۸� �ٽ� �ѽʽÿ�.
    m_D3D->TurnZBufferOn();

    // ������ �� ����� ȭ�鿡 ǥ���մϴ�.
    m_D3D->EndScene();

    return true;
}


void ApplicationClass::TestIntersection(int mouseX, int mouseY)
{
    XMMATRIX projectionMatrix, viewMatrix, inverseViewMatrix, worldMatrix, translateMatrix, inverseWorldMatrix;
    XMFLOAT3 direction, origin, rayOrigin, rayDirection;

    // ���콺 Ŀ�� ��ǥ�� -1���� +1 ������ �̵��մϴ�
    float pointX = ((2.0f * (float)mouseX) / (float)m_screenWidth) - 1.0f;
    float pointY = (((2.0f * (float)mouseY) / (float)m_screenHeight) - 1.0f) * -1.0f;

    // ����Ʈ�� ��Ⱦ�� ����Ͽ� ���� ����� ����Ͽ� ���� �����Ѵ�.
    m_D3D->GetProjectionMatrix(projectionMatrix);

    XMFLOAT3X3 projectionMatrix4;
    XMStoreFloat3x3(&projectionMatrix4, projectionMatrix);

    pointX = pointX / projectionMatrix4._11;
    pointY = pointY / projectionMatrix4._22;

    // �� ����� ���Լ��� ���Ѵ�.
    m_Camera->GetViewMatrix(viewMatrix);
    inverseViewMatrix = XMMatrixInverse(nullptr, viewMatrix);

    XMFLOAT3X3 inverseViewMatrix4;
    XMStoreFloat3x3(&inverseViewMatrix4, inverseViewMatrix);

    // �� �������� ��ŷ ������ ������ ����մϴ�.
    direction.x = (pointX * inverseViewMatrix4._11) + (pointY * inverseViewMatrix4._21) + inverseViewMatrix4._31;
    direction.y = (pointX * inverseViewMatrix4._12) + (pointY * inverseViewMatrix4._22) + inverseViewMatrix4._32;
    direction.z = (pointX * inverseViewMatrix4._13) + (pointY * inverseViewMatrix4._23) + inverseViewMatrix4._33;

    origin = m_Camera->GetPosition();

    // ���� ����� ������ ���� ��ġ�� ��ȯ�Ѵ�.
    m_D3D->GetWorldMatrix(worldMatrix);
    translateMatrix = XMMatrixTranslation(-5.0f, 1.0f, 5.0f);
    worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);


    // Translation ����� ���Լ��� ���Ѵ�.
    inverseWorldMatrix = XMMatrixInverse(nullptr, worldMatrix);


    // ���� ���� ������ ���� ������ �� �������� ���� �������� ��ȯ�մϴ�.
    XMStoreFloat3(&rayOrigin, XMVector3TransformCoord(XMVectorSet(origin.x, origin.y, origin.z, 0.0f), inverseWorldMatrix));
    XMStoreFloat3(&direction, XMVector3TransformNormal(XMVectorSet(direction.x, direction.y, direction.z, 0.0f),
        inverseWorldMatrix));

    // ���� ������ ǥ��ȭ�մϴ�.
    XMStoreFloat3(&rayDirection, XMVector3Normalize(XMVectorSet(direction.x, direction.y, direction.z, 0.0f)));

    // ���� ���� �� ���� �׽�Ʈ�� �����Ͻʽÿ�.
    if (RaySphereIntersect(rayOrigin, rayDirection, 1.0f) == true)
    {
        // �����ϴ� ��� ȭ�鿡 ǥ�õǴ� �ؽ�Ʈ ���ڿ����� �����θ� "yes"�� �����մϴ�.
        m_Text->SetIntersection(true, m_D3D->GetDeviceContext());
    }
    else
    {
        // �׷��� ������ "No"�� ������ �����Ͻʽÿ�.
        m_Text->SetIntersection(false, m_D3D->GetDeviceContext());
    }
}


bool ApplicationClass::RaySphereIntersect(XMFLOAT3 rayOrigin, XMFLOAT3 rayDirection, float radius)
{
    // a, b �� c ����� ����մϴ�.
    float a = (rayDirection.x * rayDirection.x) + (rayDirection.y * rayDirection.y) + (rayDirection.z * rayDirection.z);
    float b = ((rayDirection.x * rayOrigin.x) + (rayDirection.y * rayOrigin.y) + (rayDirection.z * rayOrigin.z)) * 2.0f;
    float c = ((rayOrigin.x * rayOrigin.x) + (rayOrigin.y * rayOrigin.y) + (rayOrigin.z * rayOrigin.z)) - (radius * radius);

    // ������� ��´�
    float discriminant = (b * b) - (4 * a * c);

    // ������� �����̸� ��ŷ ���� ���� ��� ���Դϴ�. �׷��� ������ ���� �����մϴ�.
    if (discriminant < 0.0f)
    {
        return false;
    }

    return true;
}
