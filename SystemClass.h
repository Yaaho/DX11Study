#pragma once

// 전방선언
// 헤더에 include 를 많이 할수록 컴파일 속도가 느려진다.
// 이때 전방선언을 하여 헤더에서 전방선언한 클래스를 사용한다.
// 이때 주의점이 있는데 전방선언 시 객체는 포인터로만 선언해야 한다. 
// 전방 선언 만으로는 해당 객체의 크기를 파악할 수 없기 때문


class ApplicationClass;

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
	
	ApplicationClass* m_Application = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;