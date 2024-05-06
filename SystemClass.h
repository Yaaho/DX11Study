#pragma once

// �Է��� ó���ϴ� Ŭ����
class InputClass;
// �׷����� ó���ϴ� Ŭ����
class GraphicsClass;

// WinAPI ������ ���α׷����� Ŭ���� ȭ �� ��
class SystemClass
{
public:
	SystemClass();
	// ���� ������
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	// ���� ���α׷��� ����Ǵ� ���� ���� ���α׷��� ���۵� Windows �ý��� �޽����� ó��
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