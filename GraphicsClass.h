#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class TextClass;
class ModelClass;

class DepthShaderClass;
class FireShaderClass;

class TextureShaderClass;

class HorizontalblurShaderClass;
class VerticalblurShaderClass;

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

	bool RenderSceneToTexture();
	bool DownSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool UpSampleTexture();
	bool Render2DTextureScene();







	/*
	int RenderScene();
	int RenderToFadeTexture();

	bool RenderFadingScene();
	*/
	bool RenderText(int renderCount);
	

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	TextClass* m_Text = nullptr;

	ModelClass* m_FireModel = nullptr;
	FireShaderClass* m_FireShader = nullptr;

	TextureShaderClass* m_TextureShader = nullptr;


	HorizontalblurShaderClass* m_HorizontalblurShader = nullptr;
	VerticalblurShaderClass* m_VerticalblurShader = nullptr;

	RenderTextureClass* m_RenderTexture = nullptr;
	RenderTextureClass* m_DownSampleTexure = nullptr;
	RenderTextureClass* m_HorizontalBlurTexture = nullptr;
	RenderTextureClass* m_VerticalBlurTexture = nullptr;
	RenderTextureClass* m_UpSampleTexure = nullptr;



	RenderTextureClass* m_FadeRenderTexture = nullptr;

	OrthoWindowClass* m_SmallWindow = nullptr;
	OrthoWindowClass* m_FullScreenWindow = nullptr;
	FadeShaderClass* m_FadeShader = nullptr;

	float m_fadeInTime = 0;
	float m_accumulatedTime = 0;
	float m_fadePercentage = 0;
	float m_fadeDone = false;
};