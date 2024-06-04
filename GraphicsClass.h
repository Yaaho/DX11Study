#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class TextClass;
class ModelClass;

class RenderTextureClass;

class LightShaderClass;
class LightClass;

class RefractionShaderClass;
class WaterShaderClass;

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
	bool RenderReflectionToTexture();

	bool RenderFadingScene();
	bool RenderText(int renderCount);
	

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	TextClass* m_Text = nullptr;

	ModelClass* m_GroundModel = nullptr;
	ModelClass* m_WallModel = nullptr;
	ModelClass* m_BathModel = nullptr;
	ModelClass* m_WaterModel = nullptr;

	RenderTextureClass* m_FadeRenderTexture = nullptr;
	RenderTextureClass* m_RefractionTexture = nullptr;
	RenderTextureClass* m_ReflectionTexture = nullptr;

	LightShaderClass* m_LightShader = nullptr;
	LightClass* m_Light1 = nullptr;
	LightClass* m_Light2 = nullptr;
	LightClass* m_Light3 = nullptr;
	LightClass* m_Light4 = nullptr;

	RefractionShaderClass* m_RefractionShader = nullptr;
	WaterShaderClass* m_WaterShader = nullptr;

	OrthoWindowClass* m_FullScreenWindow = nullptr;
	FadeShaderClass* m_FadeShader = nullptr;

	float m_fadeInTime = 0;
	float m_accumulatedTime = 0;
	float m_fadePercentage = 0;
	float m_fadeDone = false;

	float m_waterHeight = 0;
	float m_waterTranslation = 0;
};