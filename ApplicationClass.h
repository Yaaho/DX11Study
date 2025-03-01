#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 700.0f;
const float SCREEN_NEAR = 1.0f;


class InputClass;
class D3DClass;
class TimerClass;
class PositionClass;
class CameraClass;
class LightClass;
class SamplerClass;
class ModelClass;
class SkyboxClass;
class TextureClass;
class RenderTextureClass;
class RenderTextureCubeClass;
class RenderTexture3DClass;

class DepthShaderClass;
class GBuffersClass;
class GBufferShaderClass;
class DeferredShaderClass;
class SkyboxShaderClass;
class LightInjectionShaderClass;
class RaymarchShaderClass;
class VolumetricLightShaderClass;


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
    bool ComputeVolumetricLight();
    bool RenderVolumetricLight();

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

    SkyboxClass* m_Skybox = nullptr;
    TextureClass* m_SkyboxCubeMap = nullptr;

    TextureClass* m_BlueNoises[16] = { nullptr };

    RenderTextureClass* m_DepthMapTexture;

    RenderTextureCubeClass* m_EnvironmentMap;
    RenderTextureCubeClass* m_SpecularMap;
    RenderTextureClass* m_BrdfLUT;

    // 2개의 버퍼 리소스를 셰이더에서의 입력 리소스와 출력 리소스로 교환해가며 사용한다.
    // 스왑 체인에서 백버퍼와 프론트 버퍼를 바꾸는 것과 비슷한가?
    RenderTexture3DClass* m_TemporalIntegrationVoxelGrid[2];
    RenderTexture3DClass* m_RayMarchVoxelGrid;

    DepthShaderClass* m_DepthShader;

    GBuffersClass* m_GBuffers = nullptr;
    GBufferShaderClass* m_GBufferShader = nullptr;

    DeferredShaderClass* m_DeferredShader = nullptr;
    
    SkyboxShaderClass* m_SkyboxShader = nullptr;

    LightInjectionShaderClass* m_LightInjectionShader = nullptr;

    RaymarchShaderClass* m_RaymarchShader = nullptr;
    
    VolumetricLightShaderClass* m_VolumetricLightShader = nullptr;


    int m_ScreenWidth;
    int m_ScreenHeight;
    
    int m_FrameIndex = 0;
};
