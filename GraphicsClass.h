#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class TextClass;
class ModelClass;

class LightShaderClass;
class LightClass;

class RenderTextureClass;
class GlassShaderClass;

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
	bool Frame(float, float);
	bool Render();

private:
	int RenderScene();
	int RenderToFadeTexture();
	
	bool RenderRefractionToTexture();

	bool RenderFadingScene();
	bool RenderText(int renderCount);
	

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	TextClass* m_Text = nullptr;

	ModelClass* m_Model = nullptr;
	ModelClass* m_WindowModel = nullptr;

	LightShaderClass* m_LightShader = nullptr;
	LightClass* m_Light = nullptr;

	RenderTextureClass* m_FadeRenderTexture = nullptr;
	RenderTextureClass* m_RefractionTexture = nullptr;
	GlassShaderClass* m_GlassShader = nullptr;

	OrthoWindowClass* m_FullScreenWindow = nullptr;
	FadeShaderClass* m_FadeShader = nullptr;

	float m_fadeInTime = 0;
	float m_accumulatedTime = 0;
	float m_fadePercentage = 0;
	float m_fadeDone = false;

	float m_waterHeight = 0;
	float m_waterTranslation = 0;
};