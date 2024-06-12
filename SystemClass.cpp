#include "Stdafx.h"
#include "InputClass.h"
#include "GraphicsClass.h"
#include "TimerClass.h"
#include "PositionClass.h"
#include "SystemClass.h"


SystemClass::SystemClass()
{
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	// ������ â ����, ���� ���� ���� �ʱ�ȭ
	int screenWidth = 0;
	int screenHeight = 0;

	// ������ ���� �ʱ�ȭ
	InitializeWindows(screenWidth, screenHeight);

	// m_Input ��ü ����, ������� Ű���� �Է� ó���� ����
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// m_Input ��ü �ʱ�ȭ
	if (!m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight))
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
	}

	// m_Graphics ��ü ����. �׷��� �������� ó���ϱ� ���� ��ü
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	// m_Graphics ��ü �ʱ�ȭ
	if (!m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd))
	{
		return false;
	}

	m_Timer = new TimerClass;
	if (!m_Timer)
	{
		return true;
	}

	if (!m_Timer->Initialize())
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	m_Position = new PositionClass;
	if (!m_Position)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// ��ġ ��ü ��ȯ
	if (m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// Ÿ�̸� ��ü�� �����Ѵ�.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// m_Graphics ��ü ��ȯ
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// m_Input ��ü ��ȯ
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// Windows ���� ó��
	ShutdownWindows();
}

void SystemClass::Run()
{
	// �޽��� ����ü ���� �� �ʱ�ȭ
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// ����ڷκ��� ���� �޽����� ���� ������ �޽��� ������ ����
	while (true)
	{
		// ������ �޽����� ó���մϴ�
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// ���� �޽����� ���� ��� �޽��� ������ Ż���մϴ�
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// �� �ܿ��� Frame �Լ��� ó���մϴ�.
			if (!Frame())
			{
				break;
			}
		}
	}
}

bool SystemClass::Frame()
{
	// ����� �Է��� �н��ϴ�.
	bool result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// ����ڰ� ESCŰ�� �������� Ȯ�� �� ���� ó����
	if (m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// �ý��� ���¸� �����մϴ�.
	m_Timer->Frame();

	// ������ �Է� ó���� �����մϴ�.
	result = HandleInput(m_Timer->GetTime());
	if (!result)
	{
		return false;
	}

	// ���� ��ġ / ȸ���� �����ɴϴ�.
	XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_Position->GetPosition(pos);
	m_Position->GetRotation(rot);

	// �׷��� ��ü�� Frame �� ó���Ѵ�.
	if (!m_Graphics->Frame(m_Timer->GetTime(), pos.x, pos.y, pos.z, rot.x, rot.y, rot.z))
	{
		return false;
	}

	return m_Graphics->Render();
}


bool SystemClass::HandleInput(float frameTime)
{
	// ���ŵ� ��ġ�� ����ϱ� ���� ������ �ð��� ���� �մϴ�.
	m_Position->SetFrameTime(frameTime);

	// �Է��� ó���մϴ�.
	bool keyDown = m_Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = m_Input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	keyDown = m_Input->IsUpPressed();
	m_Position->MoveForward(keyDown);

	keyDown = m_Input->IsDownPressed();
	m_Position->MoveBackward(keyDown);

	keyDown = m_Input->IsAPressed();
	m_Position->MoveUpward(keyDown);

	keyDown = m_Input->IsZPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = m_Input->IsPgUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = m_Input->IsPgDownPressed();
	m_Position->LookDownward(keyDown);

	return true;
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	// �ܺ� �����͸� �� ��ü�� �����Ѵ�
	ApplicationHandle = this;

	// �� ���α׷��� �ν��Ͻ��� �����´�
	m_hinstance = GetModuleHandle(NULL);

	// ���α׷��� �̸��� �����Ѵ�
	m_applicationName = L"DX11Demo_16";

	// windows Ŭ������ �Ʒ��� ���� �����Ѵ�.
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// windows class �� ����մϴ�
	RegisterClassEx(&wc);

	// ����� ȭ���� �ػ󵵸� �о�ɴϴ�
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = 0;
	int posY = 0;

	// FULL_SCREEN bool ���� ���� ���� ȭ���� �����Ѵ�
	if (FULL_SCREEN)
	{
		// Ǯ��ũ�� ���� �����ߴٸ� ����� ȭ�� �ػ󵵸� ����ũž �ػ󵵷� �����ϰ� ������ 32bit ���� �����Ѵ�
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Ǯ��ũ������ ���÷��� ������ �����Ѵ�.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// ������ ����� ��� 800 * 600 ũ�⸦ �����Ѵ�
		screenWidth = 800;
		screenHeight = 600;

		// ������ â�� ����, ������ �� ��� ������ �Ѵ�
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// �����츦 �����ϰ� �ڵ��� ���մϴ�.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �����մϴ�.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);
}

void SystemClass::ShutdownWindows()
{
	// Ǯ��ũ�� ��忴�ٸ� ���÷��� ������ �ʱ�ȭ�մϴ�
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// â�� �����մϴ�
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ���α׷� �ν��Ͻ��� �����մϴ�
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// �ܺ� ������ ������ �ʱ�ȭ�մϴ�
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	// ������ ���Ḧ Ȯ���Ѵ�
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	// �����찡 �������� Ȯ���Ѵ�
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	// �� ���� ��� �޽������� �ý��� Ŭ������ �޽��� ó���� �ѱ��.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}



