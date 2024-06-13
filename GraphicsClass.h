#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;
const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;

class D3DClass;
class CameraClass;
class TextClass;

class ModelClass;

class LightClass;
class DepthShaderClass;
class ShadowShaderClass;

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
	bool Frame(float, float, float, float, float, float, float);
	bool Render();

private:
	bool RenderScene();

	bool RenderToDepthMapTexture1();
	bool RenderToDepthMapTexture2();
	bool RenderToFadeTexture();

	bool RenderFadingScene();
	bool RenderText(int renderCount);
	

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	TextClass* m_Text = nullptr;

	ModelClass* m_CubeModel = nullptr;
	ModelClass* m_GroundModel = nullptr;
	ModelClass* m_SphereModel = nullptr;

	LightClass* m_Light1 = nullptr;
	LightClass* m_Light2 = nullptr;


	DepthShaderClass* m_DepthShader = nullptr;
	ShadowShaderClass* m_ShadowShader = nullptr;
	RenderTextureClass* m_DepthMapTexture1 = nullptr;
	RenderTextureClass* m_DepthMapTexture2 = nullptr;

	RenderTextureClass* m_FadeRenderTexture = nullptr;

	OrthoWindowClass* m_FullScreenWindow = nullptr;
	FadeShaderClass* m_FadeShader = nullptr;

	float m_fadeInTime = 0;
	float m_accumulatedTime = 0;
	float m_fadePercentage = 0;
	float m_fadeDone = false;
};