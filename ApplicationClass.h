#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;


class InputClass;
class D3DClass;
class TimerClass;
class PositionClass;
class CameraClass;
class LightClass;
class SamplerClass;
class ModelClass;
class OrthoWindowClass;
class SkyboxClass;
class TextureClass;
class RenderTextureClass;
class RenderTextureCubeClass;
class DepthShaderClass;
class GBuffersClass;
class GBufferShaderClass;
class DeferredShaderClass;
class SkyboxShaderClass;


class ApplicationClass
{
public:
    ApplicationClass();
    ApplicationClass(const ApplicationClass&);
    ~ApplicationClass();

    bool Initialize(HINSTANCE, HWND, int, int);
    void Shutdown();
    bool Frame();

private:
    bool HandleMovementInput(float);
    bool Render();
    bool RenderGBuffer();
    bool RenderDepthMap();
    bool RenderEnvMap();
    bool RenderSkybox();

private:
    InputClass* m_Input = nullptr;
    D3DClass* m_Direct3D = nullptr;
    TimerClass* m_Timer = nullptr;
    PositionClass* m_Position = nullptr;
    CameraClass* m_Camera = nullptr;
    LightClass* m_Light = nullptr;
    SamplerClass* m_Sampler = nullptr;


    ModelClass* m_Cube = nullptr;
    ModelClass* m_Plane = nullptr;
  

    OrthoWindowClass* m_FullScreenWindow = nullptr;

    SkyboxClass* m_Skybox = nullptr;
    TextureClass* m_SkyboxCubeMap = nullptr;

    RenderTextureClass* m_DepthMapTexture;

    RenderTextureCubeClass* m_EnvironmentMap;
    RenderTextureCubeClass* m_SpecularMap;
    RenderTextureClass* m_BrdfLUT;

    DepthShaderClass* m_DepthShader;

    GBuffersClass* m_GBuffers = nullptr;
    GBufferShaderClass* m_GBufferShader = nullptr;

    DeferredShaderClass* m_DeferredShader = nullptr;


    SkyboxShaderClass* m_SkyboxShader = nullptr;
};
