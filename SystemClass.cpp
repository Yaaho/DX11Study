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
		// ������ �޽����� ó���Ѵ�.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// ���� �޽����� ���� ��� �޽��� ������ Ż���Ѵ�
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// �� �ܿ��� Frame �Լ��� ó���Ѵ�.
			if (!Frame())
			{
				MessageBox(m_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				break;
			}
		}
		
		// ����ڰ� ESC Ű�� �������� Ȯ�� �� ���� ó����
		if (m_Input->IsEscapePressed())
		{
			break;
		}
	}
}

bool SystemClass::Frame()
{
	XMFLOAT3 rotation(0.0f, 0.0f, 0.0f);

	// �ý��� ��踦 ������Ʈ �Ѵ�.
	m_Timer->Frame();
	
	// �Է� ������ ó���� �����Ѵ�.
	if (!m_Input->Frame())
	{
		return false;
	}

	// ������Ʈ �� ��ġ�� ����ϱ� ���� ������ �ð��� �����Ѵ�.
	m_Position->SetFrameTime(m_Timer->GetTime());

	// ���� �Ǵ� ������ ȭ��ǥ Ű�� �������� Ȯ���ϰ� ī�޶� ȸ����Ų��.
	m_Position->TurnLeft(m_Input->IsLeftArrowPressed());
	m_Position->TurnRight(m_Input->IsRightArrowPressed());

	m_Position->GetRotation(rotation);

	// �׷��� ��ü�� Frame �� ó���Ѵ�.
	if (!m_Graphics->Frame(rotation, m_Timer->GetTime()))
	{
		return false;
	}

	return m_Graphics->Render();
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



