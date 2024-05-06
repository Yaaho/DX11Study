#pragma once

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled = false;
	int m_videoCardMemory = 0;
	char m_videoCardDescription[128] = { 0, };
	IDXGISwapChain* m_swapChain = nullptr;
	// device 는 리소스 생성 할당 등의 역할을 한다.
	ID3D11Device* m_device = nullptr;
	// devicecontext 는 주로 직접 그리는 역할을 한다.
	ID3D11DeviceContext* m_deviceContext = nullptr;
	// View 란 리소스를 사용하기 위한 파생 인터페이스이고 readonly 가 기본이다. 쉐이더 안에서 쓰려면 쉐이더 리소스 뷰를 생성해야 한다.
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11RasterizerState* m_rasterState = nullptr;

	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
};