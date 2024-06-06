#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;

class D3DClass;
class CameraClass;
class TextClass;
class ModelClass;

class DepthShaderClass;
class FireShaderClass;

class RenderTextureClass;

class OrthoWindowClass;
class FadeShaderClass;


class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(XMFLOAT3&, float);
	bool Render();

private:
	int RenderScene();
	int RenderToFadeTexture();

	bool RenderFadingScene();
	bool RenderText(int renderCount);
	

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	TextClass* m_Text = nullptr;

	ModelClass* m_FloorModel = nullptr;
	ModelClass* m_FireModel = nullptr;
	
	DepthShaderClass* m_DepthShader = nullptr;
	FireShaderClass* m_FireShader = nullptr;

	RenderTextureClass* m_FadeRenderTexture = nullptr;

	OrthoWindowClass* m_FullScreenWindow = nullptr;
	FadeShaderClass* m_FadeShader = nullptr;

	float m_fadeInTime = 0;
	float m_accumulatedTime = 0;
	float m_fadePercentage = 0;
	float m_fadeDone = false;
};