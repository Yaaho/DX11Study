#pragma once

// 입력을 처리하는 클래스
class InputClass;
// 그래픽을 처리하는 클래스
class GraphicsClass;

// WinAPI 윈도우 프로그래밍을 클래스 화 한 것
class SystemClass
{
public:
	SystemClass();
	// 복사 생성자
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	// 응용 프로그램이 실행되는 동안 응용 프로그램에 전송될 Windows 시스템 메시지를 처리
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input = nullptr;
	GraphicsClass* m_Graphics = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;