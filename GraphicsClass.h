#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;


class D3DClass;
class ShaderManagerClass;

class CameraClass;
class TextClass;

class TextureShaderClass;
class BitmapClass;
class HorizontalBlurShaderClass;
class VerticalBlurShaderClass;
class GlowMapShaderClass;
class GlowShaderClass;




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


	bool RenderGlowMapToTexture();
	bool DownSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool RenderUIElementsToTexture();
	bool RenderGlowScene();




















	bool RenderToFadeTexture();

	bool RenderFadingScene();
	bool RenderText(int renderCount);
	

private:
	D3DClass* m_Direct3D = nullptr;
	ShaderManagerClass* m_ShaderManager = nullptr;

	CameraClass* m_Camera = nullptr;
	TextClass* m_Text = nullptr;

	TextureShaderClass* m_TextureShader = nullptr;
	BitmapClass* m_Bitmap = nullptr;


	HorizontalBlurShaderClass* m_HorizontalBlurShader = nullptr;
	VerticalBlurShaderClass* m_VerticalBlurShader = nullptr;
	GlowMapShaderClass* m_GlowMapShader = nullptr;
	GlowShaderClass* m_GlowShader = nullptr;



	RenderTextureClass* m_RenderTexture = nullptr;
	RenderTextureClass* m_DownSampleTexure = nullptr;
	RenderTextureClass* m_HorizontalBlurTexture = nullptr;
	RenderTextureClass* m_VerticalBlurTexture = nullptr;


	RenderTextureClass* m_FadeRenderTexture = nullptr;


	OrthoWindowClass* m_FullScreenWindow = nullptr;
	OrthoWindowClass* m_SmallWindow = nullptr;

	FadeShaderClass* m_FadeShader = nullptr;

	float m_fadeInTime = 0;
	float m_accumulatedTime = 0;
	float m_fadePercentage = 0;
	float m_fadeDone = false;
};