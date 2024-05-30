#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class D3DClass;
class CameraClass;
class TextClass;
class ModelClass;
class SpecMapShaderClass;
class LightClass;

class RenderTextureClass;
class ReflectionShaderClass;

class ModelListClass;
class FrustumClass;



class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float);
	bool Render();

private:
	bool RenderToTexture();
	int RederScene();

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	TextClass* m_Text = nullptr;
	ModelClass* m_Model = nullptr;
	SpecMapShaderClass* m_SpecMapShader = nullptr;
	LightClass* m_Light = nullptr;

	RenderTextureClass* m_RenderTexture = nullptr;
	ModelClass* m_FloorModel = nullptr;
	ReflectionShaderClass* m_ReflectionShader = nullptr;

	ModelListClass* m_ModelList = nullptr;
	FrustumClass* m_Frustum = nullptr;
};