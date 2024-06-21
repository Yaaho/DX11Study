#pragma once

// ���漱��
// ����� include �� ���� �Ҽ��� ������ �ӵ��� ��������.
// �̶� ���漱���� �Ͽ� ������� ���漱���� Ŭ������ ����Ѵ�.
// �̶� �������� �ִµ� ���漱�� �� ��ü�� �����ͷθ� �����ؾ� �Ѵ�. 
// ���� ���� �����δ� �ش� ��ü�� ũ�⸦ �ľ��� �� ���� ����


class ApplicationClass;

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
	
	ApplicationClass* m_Application = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;