#include "Stdafx.h"
#include "InputClass.h"
#include "D3DClass.h"
#include "TimerClass.h"
#include "PositionClass.h"
#include "CameraClass.h"
#include "LightClass.h"
#include "SamplerClass.h"
#include "ModelClass.h"
#include "SkyboxClass.h"
#include "TextureClass.h"
#include "RenderTextureClass.h"
#include "RenderTextureCubeClass.h"
#include "RenderTexture3DClass.h"
#include "DepthShaderClass.h"
#include "GBuffersClass.h"
#include "GBufferShaderClass.h"
#include "DeferredShaderClass.h"
#include "SkyboxShaderClass.h"
#include "LightInjectionShaderClass.h"
#include "RaymarchShaderClass.h"
#include "VolumetricLightShaderClass.h"
#include "ApplicationClass.h"


ApplicationClass::ApplicationClass()
{
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
    bool result;


    // 입력 개체를 만듭니다. 입력 객체는 사용자로부터 키보드 및 마우스 입력 읽기를 처리하는 데 사용됩니다.
    m_Input = new InputClass;
    if (!m_Input)
    {
        return false;
    }

    m_ScreenWidth = screenWidth;
    m_ScreenHeight = screenHeight;

    // 입력 개체를 초기화 합니다.
    result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
        return false;
    }

    // Direct3D 객체를 생성합니다.
    m_Direct3D = new D3DClass;
    if (!m_Direct3D)
    {
        return false;
    }

    // Direct3D 객체를 초기화 합니다.
    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
        return false;
    }


    // 타이머 개체를 만듭니다.
    m_Timer = new TimerClass;
    if (!m_Timer)
    {
        return false;
    }

    // 타이머 개체를 초기화 합니다.
    result = m_Timer->Initialize();
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
        return false;
    }

    // 위치 개체를 만듭니다.
    m_Position = new PositionClass;
    if (!m_Position)
    {
        return false;
    }

    // 초기 위치를 설정합니다.
    m_Position->SetPosition(0.0f, 7.0f, -11.0f);
    m_Position->SetRotation(0.0f, 0.0f, 0.0f);



    // 카메라 객체를 생성합니다.
    m_Camera = new CameraClass;
    if (!m_Camera)
    {
        return false;
    }

    XMFLOAT3 pos;

    m_Position->GetPosition(pos);

    // 카메라의 초기 위치를 설정하고 렌더링에 필요한 행렬을 만듭니다.
    m_Camera->SetPosition(pos);
    m_Camera->Render();
    m_Camera->RenderBaseViewMatrix();

    // 조명 객체를 생성합니다.
    m_Light = new LightClass;
    if (!m_Light->Initialize(m_Direct3D->GetDevice(), hwnd, 4096, 4096, 300.0f, 1.0f))
    {
        return false;
    }

    m_Light->m_lightProps.m_GlobalAmbient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);

    m_Light->m_lightProps.m_Lights[0].m_Position = XMFLOAT4(15.0f, 0.0f, -20.0f, 1.0f);
    m_Light->m_lightProps.m_Lights[1].m_Position = XMFLOAT4(7.0f, 5.0f, 7.0f, 1.0f);
    m_Light->m_lightProps.m_Lights[2].m_Position = XMFLOAT4(-4.0f, 5.0f, -5.0f, 1.0f);
    m_Light->m_lightProps.m_Lights[3].m_Position = XMFLOAT4(-9.0f, 3.0f, -8.0f, 1.0f);


    m_Light->m_lightProps.m_Lights[0].m_Direction = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
    m_Light->m_lightProps.m_Lights[1].m_Direction = XMFLOAT4(-7.0f, -5.0f, -7.0f, 1.0f);
    m_Light->m_lightProps.m_Lights[2].m_Direction = XMFLOAT4(4.0f, -5.0f, 5.0f, 1.0f);
    m_Light->m_lightProps.m_Lights[3].m_Direction = XMFLOAT4(9.0f, -3.0f, 8.0f, 1.0f);

    // 알파 채널은 ambient light intensity 로 쓰인다.
    m_Light->m_lightProps.m_Lights[0].m_Color = XMFLOAT4(0.7f, 0.4f, 0.2f, 0.0001f);
    m_Light->m_lightProps.m_Lights[1].m_Color = XMFLOAT4(0.4f, 0.9f, 0.6f, 0.0001f);
    m_Light->m_lightProps.m_Lights[2].m_Color = XMFLOAT4(0.7f, 0.8f, 0.9f, 0.0001f);
    m_Light->m_lightProps.m_Lights[3].m_Color = XMFLOAT4(0.9f, 0.9f, 0.9f, 0.0001f);

    m_Light->m_lightProps.m_Lights[0].m_LightType = LightType::DirectionalLight;
    m_Light->m_lightProps.m_Lights[1].m_LightType = LightType::PointLight;
    m_Light->m_lightProps.m_Lights[2].m_LightType = LightType::PointLight;
    m_Light->m_lightProps.m_Lights[3].m_LightType = LightType::PointLight;


    m_Light->m_lightProps.m_Lights[0].m_Status = LightStatus::Static_Shadow;
    m_Light->m_lightProps.m_Lights[1].m_Status = LightStatus::Enable;
    m_Light->m_lightProps.m_Lights[2].m_Status = LightStatus::Enable;
    m_Light->m_lightProps.m_Lights[3].m_Status = LightStatus::Enable;

    m_Light->m_shadowMapProps.m_ShadowMaps[0].m_ShadowMapTopLeftX = 0.0f;
    m_Light->m_shadowMapProps.m_ShadowMaps[0].m_ShadowMapTopLeftY = 0.0f;
    m_Light->m_shadowMapProps.m_ShadowMaps[0].m_ShadowMapTextureRatio = 0.5f;

    m_Light->m_shadowMapProps.m_ShadowMaps[1].m_ShadowMapTopLeftX = 0.5f;
    m_Light->m_shadowMapProps.m_ShadowMaps[1].m_ShadowMapTopLeftY = 0.0f;
    m_Light->m_shadowMapProps.m_ShadowMaps[1].m_ShadowMapTextureRatio = 0.5f;

    m_Light->m_shadowMapProps.m_ShadowMaps[2].m_ShadowMapTopLeftX = 0.0f;
    m_Light->m_shadowMapProps.m_ShadowMaps[2].m_ShadowMapTopLeftY = 0.5f;
    m_Light->m_shadowMapProps.m_ShadowMaps[2].m_ShadowMapTextureRatio = 0.5f;

    m_Light->m_shadowMapProps.m_ShadowMaps[3].m_ShadowMapTopLeftX = 0.5f;
    m_Light->m_shadowMapProps.m_ShadowMaps[3].m_ShadowMapTopLeftY = 0.5f;
    m_Light->m_shadowMapProps.m_ShadowMaps[3].m_ShadowMapTextureRatio = 0.5f;



    
    m_Sampler = new SamplerClass;
    if (!m_Sampler->Initialize(m_Direct3D->GetDevice()))
    {
        return false;
    }


    // 모델 객체를 생성합니다.
    m_Cube = new ModelClass;
    if (!m_Cube)
    {
        return false;
    }


    // 그라운드 모델의 위치를 ?설정합니다.
    result = m_Cube->Initialize(m_Direct3D->GetDevice(), "data/cube.txt", 1.0f);
    // 1 알베도, 2 노말, 3 오클루전 러프니스 메탈, 4 AO, 5 이미시브
    result = m_Cube->LoadTextures(m_Direct3D->GetDevice(), 
        L"data/stone01.dds", L"data/normal.dds", L"data/normal.dds", L"data/normal.dds", L"data/normal.dds");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    m_Cube->SetPosition(XMFLOAT3(-2.0f, 2.0f, 0.0f));


    m_Plane = new ModelClass;
    if (!m_Plane)
    {
        return false;
    }

    result = m_Plane->Initialize(m_Direct3D->GetDevice(), "data/plane01.txt", 1.0f);
    result = m_Plane->LoadTextures(m_Direct3D->GetDevice(),
        L"data/stone01.dds", L"data/normal.dds", L"data/normal.dds", L"data/normal.dds", L"data/normal.dds");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    m_Plane->SetPosition(XMFLOAT3(0.0f, 1.0f, 0.0f));

    m_Skybox = new SkyboxClass;
    if (!m_Skybox)
    {
        return false;
    }

    result = m_Skybox->Initialize(m_Direct3D->GetDevice());
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the skybox object.", L"Error", MB_OK);
        return false;
    }

    m_SkyboxCubeMap = new TextureClass;
    if (!m_SkyboxCubeMap)
    {
        return false;
    }

    result = m_SkyboxCubeMap->Initialize(m_Direct3D->GetDevice(), L"data/sky.dds");
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the skybox texture object.", L"Error", MB_OK);
        return false;
    }

    for (int i = 0; i < 16; i++)
    {
        m_BlueNoises[i] = new TextureClass;
        if (!m_BlueNoises[i])
        {
            return false;
        }

        std::wstringstream wss;
        wss << L"data/LDR_LLL1_" << i << L".dds";

        result = m_BlueNoises[i]->Initialize(m_Direct3D->GetDevice(), wss.str().c_str());
        if (!result)
        {
            MessageBox(hwnd, L"Could not initialize the bluenoise texture object.", L"Error", MB_OK);
            return false;
        }
    }

    // 렌더링을 텍스처 오브젝트에 생성한다.
    m_DepthMapTexture = new RenderTextureClass;
    if (!m_DepthMapTexture)
    {
        return false;
    }

    // 렌더링을 텍스처 오브젝트에 초기화한다.
    if (!m_DepthMapTexture->Initialize(m_Direct3D->GetDevice(), 4096, 4096))
    {
        MessageBox(hwnd, L"Could not initialize the depth map texture object.", L"Error", MB_OK);
        return false;
    }

    m_EnvironmentMap = new RenderTextureCubeClass;
    if (!m_EnvironmentMap)
    {
        return false;
    }

    if (!m_EnvironmentMap->Initialize(m_Direct3D->GetDevice(), 512))
    {
        MessageBox(hwnd, L"Could not initialize the environment Map object.", L"Error", MB_OK);
        return false;
    }

    m_SpecularMap = new RenderTextureCubeClass;
    if (!m_SpecularMap)
    {
        return false;
    }

    if (!m_SpecularMap->Initialize(m_Direct3D->GetDevice(), 512))
    {
        MessageBox(hwnd, L"Could not initialize the specular map object.", L"Error", MB_OK);
        return false;
    }

    m_BrdfLUT = new RenderTextureClass;
    if (!m_BrdfLUT)
    {
        return false;
    }

    if (!m_BrdfLUT->Initialize(m_Direct3D->GetDevice(), 512, 512))
    {
        MessageBox(hwnd, L"Could not initialize the brdfLUT object.", L"Error", MB_OK);
        return false;
    }

    for (int i = 0; i < 2; i++)
    {
        m_TemporalIntegrationVoxelGrid[i] = new RenderTexture3DClass;
        if (!m_TemporalIntegrationVoxelGrid[i])
        {
            return false;
        }
        
        if (!m_TemporalIntegrationVoxelGrid[i]->Initialize(m_Direct3D->GetDevice(), screenWidth / 8, screenHeight / 8, 128))
        {
            MessageBox(hwnd, L"Could not initialize the temporal integration voxel grid object.", L"Error", MB_OK);
            return false;
        }
    }

    m_RayMarchVoxelGrid = new RenderTexture3DClass;
    if (!m_RayMarchVoxelGrid)
    {
        return false;
    }
    
    if (!m_RayMarchVoxelGrid->Initialize(m_Direct3D->GetDevice(), screenWidth / 8, screenHeight / 8, 128))
    {
        MessageBox(hwnd, L"Could not initialize the raymarch voxel grid object.", L"Error", MB_OK);
        return false;
    }

    m_DepthShader = new DepthShaderClass;
    if (!m_DepthShader)
    {
        return false;
    }

    result = m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
        return false;
    }

    // 지연된 버퍼 객체를 생성합니다.
    m_GBuffers = new GBuffersClass;
    if (!m_GBuffers)
    {
        return false;
    }

    // 지연된 버퍼 객체를 초기화 합니다.
    result = m_GBuffers->Initialize(m_Direct3D->GetDevice(), screenWidth, screenHeight);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the deferred buffers object.", L"Error", MB_OK);
        return false;
    }

    // 지연된 셰이더 객체를 생성합니다.
    m_GBufferShader = new GBufferShaderClass;
    if (!m_GBufferShader)
    {
        return false;
    }

    // 지연된 셰이더 객체를 초기화 합니다.
    result = m_GBufferShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the gbuffer shader object.", L"Error", MB_OK);
        return false;
    }

    // 조명 쉐이더 객체를 생성합니다.
    m_DeferredShader = new DeferredShaderClass;
    if (!m_DeferredShader)
    {
        return false;
    }

    // 조명 쉐이더 객체를 초기화 합니다.
    result = m_DeferredShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the deferred shader object.", L"Error", MB_OK);
        return false;
    }

    m_SkyboxShader = new SkyboxShaderClass;
    if (!m_SkyboxShader)
    {
        return false;
    }

    result = m_SkyboxShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the skybox shader object.", L"Error", MB_OK);
        return false;
    }

    m_LightInjectionShader = new LightInjectionShaderClass;
    if (!m_LightInjectionShader)
    {
        return false;
    }

    // visual studio 가서 무슨 이 hlsl 파일에 대한 설정을 바꿔줘야 한다
    result = m_LightInjectionShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the light injection shader object.", L"Error", MB_OK);
        return false;
    }

    m_RaymarchShader = new RaymarchShaderClass;
    if (!m_RaymarchShader)
    {
        return false;
    }

    result = m_RaymarchShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the raymarch shader object.", L"Error", MB_OK);
        return false;
    }

    m_VolumetricLightShader = new VolumetricLightShaderClass;
    if (!m_VolumetricLightShader)
    {
        return false;
    }

    result = m_VolumetricLightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if (!result)
    {
        MessageBox(hwnd, L"Could not initialize the volumetric light shader object.", L"Error", MB_OK);
        return false;
    }

    RenderEnvMap();

    return true;
}


void ApplicationClass::Shutdown()
{
    if (m_VolumetricLightShader)
    {
        m_VolumetricLightShader->Shutdown();
        delete m_VolumetricLightShader;
        m_VolumetricLightShader = 0;
    }
    
    if (m_RaymarchShader)
    {
        m_RaymarchShader->Shutdown();
        delete m_RaymarchShader;
        m_RaymarchShader = 0;
    }
    
    if (m_LightInjectionShader)
    {
        m_LightInjectionShader->Shutdown();
        delete m_LightInjectionShader;
        m_LightInjectionShader = 0;
    }
    
    // 스카이박스 쉐이더 객체를 해제합니다.
    if (m_SkyboxShader)
    {
        m_SkyboxShader->Shutdown();
        delete m_SkyboxShader;
        m_SkyboxShader = 0;
    }

    // 조명 쉐이더 객체를 해제합니다.
    if (m_DeferredShader)
    {
        m_DeferredShader->Shutdown();
        delete m_DeferredShader;
        m_DeferredShader = 0;
    }

    // 지연된 쉐이더 객체를 해제합니다.
    if (m_GBufferShader)
    {
        m_GBufferShader->Shutdown();
        delete m_GBufferShader;
        m_GBufferShader = 0;
    }

    // 지연된 버퍼 객체를 해제합니다.
    if (m_GBuffers)
    {
        m_GBuffers->Shutdown();
        delete m_GBuffers;
        m_GBuffers = 0;
    }

    if (m_DepthShader)
    {
        m_DepthShader->Shutdown();
        delete m_DepthShader;
        m_DepthShader = 0;
    }

    if (m_RayMarchVoxelGrid)
    {
        m_RayMarchVoxelGrid->Shutdown();
        delete m_RayMarchVoxelGrid;
        m_RayMarchVoxelGrid = 0;
    }
    
    for (int i = 1; i >= 0; i--)
    {
        if (m_TemporalIntegrationVoxelGrid[i])
        {
            m_TemporalIntegrationVoxelGrid[i]->Shutdown();
            delete m_TemporalIntegrationVoxelGrid[i];
            m_TemporalIntegrationVoxelGrid[i] = 0;
        }
    }

    if (m_BrdfLUT)
    {
        m_BrdfLUT->Shutdown();
        delete m_BrdfLUT;
        m_BrdfLUT = 0;
    }

    if (m_SpecularMap)
    {
        m_SpecularMap->Shutdown();
        delete m_SpecularMap;
        m_SpecularMap = 0;
    }

    if (m_EnvironmentMap)
    {
        m_EnvironmentMap->Shutdown();
        delete m_EnvironmentMap;
        m_EnvironmentMap = 0;
    }

    if (m_DepthMapTexture)
    {
        m_DepthMapTexture->Shutdown();
        delete m_DepthMapTexture;
        m_DepthMapTexture = 0;
    }

    for (int i = 15; i >= 0; i--)
    {
        if (m_BlueNoises[i])
        {
            m_BlueNoises[i]->Shutdown();
            delete m_BlueNoises[i];
            m_BlueNoises[i] = 0;
        }
    }

    if (m_SkyboxCubeMap)
    {
        m_SkyboxCubeMap->Shutdown();
        delete m_SkyboxCubeMap;
        m_SkyboxCubeMap = 0;
    }

    if (m_Skybox)
    {
        m_Skybox->Shutdown();
        delete m_Skybox;
        m_Skybox = 0;
    }

    if (m_Plane)
    {
        m_Plane->Shutdown();
        delete m_Plane;
        m_Plane = 0;
    }

    // 모델 객체를 해제합니다.
    if (m_Cube)
    {
        m_Cube->Shutdown();
        delete m_Cube;
        m_Cube = 0;
    }

    if (m_Sampler)
    {
        m_Sampler->Shutdown();
        delete m_Sampler;
        m_Sampler = 0;
    }

    // 조명 객체를 해제합니다.
    if (m_Light)
    {
        m_Light->Shutdown();
        delete m_Light;
        m_Light = 0;
    }

    // 카메라 객체를 해제합니다.
    if (m_Camera)
    {
        delete m_Camera;
        m_Camera = 0;
    }

    // 위치 개체를 해제합니다.
    if (m_Position)
    {
        delete m_Position;
        m_Position = 0;
    }

    // 타이머 개체를 해제합니다.
    if (m_Timer)
    {
        delete m_Timer;
        m_Timer = 0;
    }

    // D3D 객체를 해제합니다.
    if (m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = 0;
    }

    // 입력 개체를 해제합니다.
    if (m_Input)
    {
        m_Input->Shutdown();
        delete m_Input;
        m_Input = 0;
    }
}

bool ApplicationClass::Frame()
{
    // 시스템의 통계를 업데이트 합니다.
    m_Timer->Frame();

    // 사용자 입력을 읽습니다.
    if (!m_Input->Frame())
    {
        return false;
    }

    // 사용자가 ESC 키를 누르고 응용 프로그램을 종료할 것인지 확인합니다.
    if (m_Input->IsEscapePressed() == true)
    {
        return false;
    }

    // 프레임 입력 처리를 수행한다.
    if (!HandleMovementInput(m_Timer->GetTime()))
    {
        return false;
    }

    if (m_Light)
    {
        m_Light->m_lightProps.m_EyePosition = XMFLOAT4(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z, 1.0f);
        m_Light->UpdateBuffer(m_Direct3D->GetDeviceContext());
    }

    // 그래픽을 렌더링 한다.
    Render();

    m_FrameIndex++;

    return true;

    /*
    초당 프레임 수를 설정한다.
    if (!m_Text->SetFps(fps, m_Direct3D->GetDeviceContext()))
    {
        return false;
    }

    cpu 사용을 설정한다.
    if (!m_Text->SetCpu(cpu, m_Direct3D->GetDeviceContext()))
    {
        return false;
    }
    */
}


bool ApplicationClass::HandleMovementInput(float frameTime)
{
    XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3 rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

    // 갱신된 위치를 계산하기 위한 프레임 시간을 설정합니다.
    m_Position->SetFrameTime(frameTime);

    // 입력을 처리합니다.
    m_Position->TurnLeft(m_Input->IsLeftPressed());
    m_Position->TurnRight(m_Input->IsRightPressed());
    m_Position->MoveForward(m_Input->IsUpPressed());
    m_Position->MoveBackward(m_Input->IsDownPressed());
    m_Position->MoveUpward(m_Input->IsAPressed());
    m_Position->MoveDownward(m_Input->IsZPressed());
    m_Position->LookUpward(m_Input->IsPgUpPressed());
    m_Position->LookDownward(m_Input->IsPgDownPressed());

    // 시점 위치 / 회전을 가져옵니다.
    m_Position->GetPosition(pos);
    m_Position->GetRotation(rot);

    // 카메라의 위치를 설정합니다.
    m_Camera->SetPosition(pos);
    m_Camera->SetRotation(rot);

    return true;
}



bool ApplicationClass::Render()
{
    XMMATRIX viewMatrix, projectionMatrix;

    // 먼저 장면을 텍스처로 렌더링합니다.
    if (!RenderGBuffer())
    {
        return false;
    }

    // 그림자 그리기에 사용할 DepthMap
    if (!RenderDepthMap())
    {
        return false;
    }
    
    if (!ComputeVolumetricLight())
    {
        return false;
    }
    
    // 장면을 시작할 버퍼를 지운다.
    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    // 모든 2D 렌더링을 시작하려면 Z 버퍼를 끕니다.
    m_Direct3D->TurnZBufferOff();


    // s0
    m_Sampler->UsePSLinear(m_Direct3D->GetDeviceContext(), 0);

    // s1
    m_Sampler->UsePSPoint(m_Direct3D->GetDeviceContext(), 1);

    // b1
    m_Light->UsePSLightBuffer(m_Direct3D->GetDeviceContext(), 1);

    // b2
    m_Light->UsePSShadowMapBuffer(m_Direct3D->GetDeviceContext(), 2);

    // t4
    m_DepthMapTexture->UsePSShaderResourceView(m_Direct3D->GetDeviceContext(), 4);

    // t9
    m_RayMarchVoxelGrid->UsePSShaderResourceView(m_Direct3D->GetDeviceContext(), 9);


    m_EnvironmentMap->UseShaderResourceView(m_Direct3D->GetDeviceContext(), 6);
    m_SpecularMap->UseShaderResourceView(m_Direct3D->GetDeviceContext(), 7);
    m_BrdfLUT->UsePSShaderResourceView(m_Direct3D->GetDeviceContext(), 8);


    m_Direct3D->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_DeferredShader->Render(m_Direct3D->GetDeviceContext(), 
        XMMatrixInverse(nullptr, projectionMatrix), XMMatrixInverse(nullptr, viewMatrix), false, 1,
        m_GBuffers->GetDepthResourceView(), 
        m_GBuffers->GetShaderResourceView(0), m_GBuffers->GetShaderResourceView(1), m_GBuffers->GetShaderResourceView(2), m_GBuffers->GetShaderResourceView(3));

    // 모든 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 켜십시오.
    m_Direct3D->TurnZBufferOn();

    // Skybox 를 그리기 위해 GBuffers 렌더링에 사용된 DepthStencil 을 다시 설정.
    // 이걸 depth stencil 로 지정하면 Volumetric light 의 셰이더 리소스로 사용할 수 없기 때문에 일단 주석처리 해 뒀다.
    // m_Direct3D->SetBackBufferRenderTargetWithStencil(m_GBuffers->GetDepthStencilView());
    // 
    // RenderSkybox();

    RenderVolumetricLight();

    // 렌더링 된 장면을 화면에 표시합니다.
    m_Direct3D->EndScene();

    return true;
}


bool ApplicationClass::RenderGBuffer()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

    XMFLOAT3 Pos;


    // 렌더링 대상을 텍스처에 렌더링으로 설정합니다.
    m_GBuffers->SetRenderTargets(m_Direct3D->GetDeviceContext());

    // 렌더링에 텍스처를 지웁니다.
    m_GBuffers->ClearRenderTargets(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

    // view matrix 갱신
    m_Camera->Render();

    // d3d 객체에서 월드 행렬을 가져옵니다.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);



    
    m_Cube->GetPosition(Pos);
    Pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    worldMatrix = XMMatrixScaling(50.0f, 10.0f, 1.0f);
    worldMatrix *= XMMatrixTranslation(Pos.x, Pos.y, Pos.z);


    // 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
    m_Cube->Render(m_Direct3D->GetDeviceContext());

    // 광원 CBuffer 는 1번 버퍼 슬롯을 사용
    m_Light->UsePSLightBuffer(m_Direct3D->GetDeviceContext(), 1);

    m_Sampler->UsePSLinear(m_Direct3D->GetDeviceContext(), 0);
    m_Sampler->UsePSPoint(m_Direct3D->GetDeviceContext(), 1);


    XMFLOAT4 gAlbedo = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    float gMetallic = 1.0f;
    float gRoughness = 0.3f;

    // 알베도 맵 사용
    int gUseAlbedoMap = 1;
    int gUseOccMetalRough = 0;
    int gUseAoMap = 0;
    int gUseEmmisive = 0;
    // 노말 맵 사용
    int gNormalState = 1;
    int gConvertToLinear = 0;


    // 지연된 셰이더를 사용하여 모델을 렌더링합니다.
    m_GBufferShader->Render(m_Direct3D->GetDeviceContext(), m_Cube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Cube->GetTexture(0), m_Cube->GetTexture(1), m_Cube->GetTexture(2), m_Cube->GetTexture(3), m_Cube->GetTexture(4),
        gAlbedo, gMetallic, gRoughness, gUseAlbedoMap, gUseOccMetalRough, gUseAoMap, gUseEmmisive, gNormalState, gConvertToLinear);












    



    m_Cube->GetPosition(Pos);
    Pos = XMFLOAT3(120.0f, 0.0f, 0.0f);
    worldMatrix = XMMatrixScaling(40.0f, 10.0f, 1.0f);
    worldMatrix *= XMMatrixTranslation(Pos.x, Pos.y, Pos.z);


    // 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
    m_Cube->Render(m_Direct3D->GetDeviceContext());

    // 광원 CBuffer 는 1번 버퍼 슬롯을 사용
    m_Light->UsePSLightBuffer(m_Direct3D->GetDeviceContext(), 1);

    m_Sampler->UsePSLinear(m_Direct3D->GetDeviceContext(), 0);
    m_Sampler->UsePSPoint(m_Direct3D->GetDeviceContext(), 1);


    gAlbedo = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    gMetallic = 1.0f;
    gRoughness = 0.3f;

    // 알베도 맵 사용
    gUseAlbedoMap = 1;
    gUseOccMetalRough = 0;
    gUseAoMap = 0;
    gUseEmmisive = 0;
    // 노말 맵 사용
    gNormalState = 1;
    gConvertToLinear = 0;


    // 지연된 셰이더를 사용하여 모델을 렌더링합니다.
    m_GBufferShader->Render(m_Direct3D->GetDeviceContext(), m_Cube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Cube->GetTexture(0), m_Cube->GetTexture(1), m_Cube->GetTexture(2), m_Cube->GetTexture(3), m_Cube->GetTexture(4),
        gAlbedo, gMetallic, gRoughness, gUseAlbedoMap, gUseOccMetalRough, gUseAoMap, gUseEmmisive, gNormalState, gConvertToLinear);









    




    m_Cube->GetPosition(Pos);
    Pos = XMFLOAT3(45.0f, 10.0f, 50.0f);
    worldMatrix = XMMatrixScaling(100.0f, 1.0f, 50.0f);
    worldMatrix *= XMMatrixTranslation(Pos.x, Pos.y, Pos.z);


    // 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
    m_Cube->Render(m_Direct3D->GetDeviceContext());

    // 광원 CBuffer 는 1번 버퍼 슬롯을 사용
    m_Light->UsePSLightBuffer(m_Direct3D->GetDeviceContext(), 1);

    m_Sampler->UsePSLinear(m_Direct3D->GetDeviceContext(), 0);
    m_Sampler->UsePSPoint(m_Direct3D->GetDeviceContext(), 1);


    gAlbedo = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    gMetallic = 1.0f;
    gRoughness = 0.3f;

    // 알베도 맵 사용
    gUseAlbedoMap = 1;
    gUseOccMetalRough = 0;
    gUseAoMap = 0;
    gUseEmmisive = 0;
    // 노말 맵 사용
    gNormalState = 1;
    gConvertToLinear = 0;


    // 지연된 셰이더를 사용하여 모델을 렌더링합니다.
    m_GBufferShader->Render(m_Direct3D->GetDeviceContext(), m_Cube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Cube->GetTexture(0), m_Cube->GetTexture(1), m_Cube->GetTexture(2), m_Cube->GetTexture(3), m_Cube->GetTexture(4),
        gAlbedo, gMetallic, gRoughness, gUseAlbedoMap, gUseOccMetalRough, gUseAoMap, gUseEmmisive, gNormalState, gConvertToLinear);




















    





    
    // 회전값으로 월드 행렬을 회전하여 큐브가 회전하도록 합니다.
    m_Direct3D->GetWorldMatrix(worldMatrix);

    Pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_Plane->GetPosition(Pos);
    worldMatrix = XMMatrixTranslation(Pos.x, Pos.y, Pos.z);


    // 모델 버텍스와 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 드로잉을 준비합니다.
    m_Plane->Render(m_Direct3D->GetDeviceContext());

    // 광원 CBuffer 는 1번 버퍼 슬롯을 사용
    m_Light->UsePSLightBuffer(m_Direct3D->GetDeviceContext(), 1);

    m_Sampler->UsePSLinear(m_Direct3D->GetDeviceContext(), 0);
    m_Sampler->UsePSPoint(m_Direct3D->GetDeviceContext(), 1);


    gAlbedo = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
    gMetallic = 0.0f;
    gRoughness = 1.0f;

    // 알베도 맵 사용
    gUseAlbedoMap = 1;
    gUseOccMetalRough = 0;
    gUseAoMap = 0;
    gUseEmmisive = 0;
    // 노말 맵 사용
    gNormalState = 0;
    gConvertToLinear = 0;


    // 지연된 셰이더를 사용하여 모델을 렌더링합니다.
    m_GBufferShader->Render(m_Direct3D->GetDeviceContext(), m_Plane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
        m_Plane->GetTexture(0), m_Plane->GetTexture(1), m_Plane->GetTexture(2), m_Plane->GetTexture(3), m_Plane->GetTexture(4),
        gAlbedo, gMetallic, gRoughness, gUseAlbedoMap, gUseOccMetalRough, gUseAoMap, gUseEmmisive, gNormalState, gConvertToLinear);



    // 렌더 버퍼가 아닌 원래의 백 버퍼로 렌더링 타겟을 다시 설정합니다.
    m_Direct3D->SetBackBufferRenderTarget();

    // 뷰포트를 원본으로 다시 설정합니다.
    m_Direct3D->ResetViewport();

    return true;
}



bool ApplicationClass::RenderDepthMap()
{
    XMMATRIX worldMatrix, lightViewProjectionMatrix;

    XMFLOAT3 Pos;

    // 렌더링 대상을 렌더링에 맞게 설정합니다.
    m_DepthMapTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

    // 렌더링 대상을 클리어
    // 최대 거리를 의미하는 1.0 으로 클리어한다.
    m_DepthMapTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        m_DepthMapTexture->SetViewports(m_Direct3D->GetDeviceContext(), 
            m_Light->m_shadowMapProps.m_ShadowMaps[i].m_ShadowMapTopLeftX, m_Light->m_shadowMapProps.m_ShadowMaps[i].m_ShadowMapTopLeftY,
            m_Light->m_shadowMapProps.m_ShadowMaps[i].m_ShadowMapTextureRatio);

        m_Direct3D->GetWorldMatrix(worldMatrix);

        lightViewProjectionMatrix = m_Light->m_shadowMapProps.m_ShadowMaps[i].m_lightViewProjection;








        
        m_Cube->GetPosition(Pos);
        Pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
        worldMatrix = XMMatrixScaling(50.0f, 10.0f, 1.0f);
        worldMatrix *= XMMatrixTranslation(Pos.x, Pos.y, Pos.z);


        // 깊이 셰이더로 큐브 모델을 렌더링합니다.
        m_Cube->Render(m_Direct3D->GetDeviceContext());
        bool result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Cube->GetIndexCount(), worldMatrix,
            lightViewProjectionMatrix);
        if (!result)
        {
            return false;
        }









        
        m_Cube->GetPosition(Pos);
        Pos = XMFLOAT3(120.0f, 0.0f, 0.0f);
        worldMatrix = XMMatrixScaling(40.0f, 10.0f, 1.0f);
        worldMatrix *= XMMatrixTranslation(Pos.x, Pos.y, Pos.z);


        // 깊이 셰이더로 큐브 모델을 렌더링합니다.
        m_Cube->Render(m_Direct3D->GetDeviceContext());
        result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Cube->GetIndexCount(), worldMatrix,
            lightViewProjectionMatrix);
        if (!result)
        {
            return false;
        }








        m_Cube->GetPosition(Pos);
        Pos = XMFLOAT3(45.0f, 10.0f, 50.0f);
        worldMatrix = XMMatrixScaling(100.0f, 1.0f, 50.0f);
        worldMatrix *= XMMatrixTranslation(Pos.x, Pos.y, Pos.z);


        // 깊이 셰이더로 큐브 모델을 렌더링합니다.
        m_Cube->Render(m_Direct3D->GetDeviceContext());
        result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Cube->GetIndexCount(), worldMatrix,
            lightViewProjectionMatrix);
        if (!result)
        {
            return false;
        }











        // 월드 행렬을 재설정합니다.
        m_Direct3D->GetWorldMatrix(worldMatrix);

        m_Plane->GetPosition(Pos);
        worldMatrix = XMMatrixTranslation(Pos.x, Pos.y, Pos.z);

        // 깊이 셰이더로 그라운드 모델을 렌더링합니다.
        m_Plane->Render(m_Direct3D->GetDeviceContext());
        result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Plane->GetIndexCount(), worldMatrix,
            lightViewProjectionMatrix);
        if (!result)
        {
            return false;
        }
    }

    // 렌더링 대상을 원래의 백 버퍼로 다시 설정한다.
    m_Direct3D->SetBackBufferRenderTarget();

    // 뷰포트를 원본으로 다시 설정합니다.
    m_Direct3D->ResetViewport();

    return true;
}



bool ApplicationClass::RenderEnvMap()
{
    XMVECTOR forward[6] =
    {
        { 1, 0, 0, 0 },
        { -1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, -1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, -1, 0 },
    };

    XMVECTOR up[6] =
    {
        { 0, 1, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, -1, 0 },
        { 0, 0, 1, 0 },
        { 0, 1, 0,  0 },
        { 0, 1, 0, 0 },
    };

    m_EnvironmentMap->SetViewports(m_Direct3D->GetDeviceContext());

    for (int i = 0; i < 6; ++i)
    {
        m_EnvironmentMap->SetRenderTarget(m_Direct3D->GetDeviceContext(), i);

        XMVECTOR m_EyePosition = XMVectorSet(0, 0, 0, 0);
        XMVECTOR m_LookAt = forward[i];
        XMVECTOR m_Up = up[i];

        float nearZ = 0;
        float farZ = 10;
        float viewWidth = 2;
        float viewHeight = 2;

        m_Skybox->Render(m_Direct3D->GetDeviceContext());
        m_SkyboxShader->RenderEnvMap(m_Direct3D->GetDeviceContext(), m_Skybox->GetIndexCount(),
            XMMatrixIdentity(),
            XMMatrixLookAtLH(m_EyePosition, m_LookAt, m_Up),
            XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ),
            m_SkyboxCubeMap->GetTexture());
    }


    int mapSize = 512;


    for (int i = 0; i < 6; ++i)
    {
        float roughness = (float)i / 5.0;
        m_SpecularMap->SetViewports(m_Direct3D->GetDeviceContext());

        for (int j = 0; j < 6; ++j)
        {
            m_SpecularMap->SetRenderTarget(m_Direct3D->GetDeviceContext(), i * 6 + j);


            XMVECTOR m_EyePosition = XMVectorSet(0, 0, 0, 0);
            XMVECTOR m_LookAt = forward[j];
            XMVECTOR m_Up = up[j];
            float nearZ = 0;
            float farZ = 10;
            float viewWidth = 2;
            float viewHeight = 2;

            m_Skybox->Render(m_Direct3D->GetDeviceContext());
            m_SkyboxShader->RenderEnvPreFilter(m_Direct3D->GetDeviceContext(), m_Skybox->GetIndexCount(),
                XMMatrixIdentity(),
                XMMatrixLookAtLH(m_EyePosition, m_LookAt, m_Up),
                XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ), 
                m_SkyboxCubeMap->GetTexture(), roughness);
        }

        mapSize /= 2;
    }


    m_BrdfLUT->SetRenderTarget(m_Direct3D->GetDeviceContext());

    m_Direct3D->TurnZBufferOff();

    m_Direct3D->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_SkyboxShader->RenderBrdfLUT(m_Direct3D->GetDeviceContext());

    m_Direct3D->TurnZBufferOn();

    // 렌더 버퍼가 아닌 원래의 백 버퍼로 렌더링 타겟을 다시 설정합니다.
    m_Direct3D->SetBackBufferRenderTarget();

    // 뷰포트를 원본으로 다시 설정합니다.
    m_Direct3D->ResetViewport();


    return true;
}



bool ApplicationClass::RenderSkybox()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    // skybox 를 큰 크기로 설정
    // 너무 작으면 물체가 조금만 떨어져도 skybox 에 의해 잘리게 됨
    XMMATRIX scaleMatrix = XMMatrixScaling(300.0f, 300.0f, 300.0f);

    // d3d 객체에서 월드 행렬을 가져옵니다.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_Position->GetPosition(pos);
    worldMatrix = XMMatrixMultiply(scaleMatrix, XMMatrixTranslation(pos.x, pos.y, pos.z));


    m_Skybox->Render(m_Direct3D->GetDeviceContext());
    m_SkyboxShader->RenderSkybox(m_Direct3D->GetDeviceContext(), m_Skybox->GetIndexCount(), 
        worldMatrix, viewMatrix, projectionMatrix, m_SkyboxCubeMap->GetTexture());

    return true;
}



bool ApplicationClass::ComputeVolumetricLight()
{
    static bool flag = false;
    ID3D11ShaderResourceView* nullsrv = NULL;
    flag = !flag;
    
    /////// RaymarchShader 실행 부분

    // 이전 프레임의 SRV 해제
    m_Direct3D->GetDeviceContext()->CSSetShaderResources(0, 1, &nullsrv);
    
    // u0
    // 이전 프레임의 SRV 는 현재 프레임의 UAV
    m_TemporalIntegrationVoxelGrid[flag ? 1 : 0]->SetUnorderedAccessView(m_Direct3D->GetDeviceContext());

    
    XMMATRIX viewMatrix, projectionMatrix, invViewMatrix, invProjectionMatrix;
    XMFLOAT4 cameraPos;
    XMFLOAT4 biasNearFarPow, anisoDensityScatteringAbsorption;
    
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    if (!m_FrameIndex)
    {
        m_LightInjectionShader->m_VolumetricProps.prevView = XMMatrixTranspose(viewMatrix);
        m_LightInjectionShader->m_VolumetricProps.prevProjection = XMMatrixTranspose(projectionMatrix);
    }
    
    invViewMatrix = XMMatrixInverse(nullptr, viewMatrix);

    invProjectionMatrix = XMMatrixInverse(nullptr, projectionMatrix);

    cameraPos = XMFLOAT4(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z, 1.0f);
    
    // 0.002f 는 bias 값
    biasNearFarPow = XMFLOAT4(0.002f, SCREEN_NEAR, SCREEN_DEPTH, 2.0f);

    // 0.2f 는 anisotropy 값
    // 2.0f 는 density
    anisoDensityScatteringAbsorption = XMFLOAT4(0.2f, 2.0f, 0.0f, 0.0f);

    // s0
    m_Sampler->UseCSPoint(m_Direct3D->GetDeviceContext(), 0);
    
    // b1
    // 광원의 정보 사용
    m_Light->UseCSLightBuffer(m_Direct3D->GetDeviceContext(), 1);

    // b2
    // 광원의 shadowmap 정보 사용
    m_Light->UseCSShadowMapBuffer(m_Direct3D->GetDeviceContext(), 2);
    
    // t0
    // 이전 프레임의 UAV 는 현재 프레임의 SRV
    m_TemporalIntegrationVoxelGrid[flag ? 0 : 1]->UseCSShaderResourceView(m_Direct3D->GetDeviceContext(), 0);
    
    // t1
    // shadow map 사용
    m_DepthMapTexture->UseCSShaderResourceView(m_Direct3D->GetDeviceContext(), 1);

    const uint32_t LOCAL_SIZE_X = 8;
    const uint32_t LOCAL_SIZE_Y = 8;
    const uint32_t LOCAL_SIZE_Z = 1;

    uint32_t size_x = static_cast<uint32_t>(ceil(float(m_ScreenWidth / 8) / float(LOCAL_SIZE_X)));
    uint32_t size_y = static_cast<uint32_t>(ceil(float(m_ScreenHeight / 8) / float(LOCAL_SIZE_Y)));
    uint32_t size_z = static_cast<uint32_t>(ceil(float(128) / float(LOCAL_SIZE_Z)));

    
    // m_LightInjectionShader 계산
    m_LightInjectionShader->Compute(m_Direct3D->GetDeviceContext(),
        invViewMatrix, invProjectionMatrix,
        cameraPos, biasNearFarPow, anisoDensityScatteringAbsorption, m_BlueNoises[m_FrameIndex % 16]->GetTexture(),
        m_FrameIndex == 0 ? 0 : 1,
        size_x, size_y, size_z);

    // PrevViewProjection 에 현재 viewprojectionmatrix 저장
    m_LightInjectionShader->m_VolumetricProps.prevView = XMMatrixTranspose(viewMatrix);
    m_LightInjectionShader->m_VolumetricProps.prevProjection = XMMatrixTranspose(projectionMatrix);

    /////// RaymarchShader 실행 부분

    // UAV 설정
    m_RayMarchVoxelGrid->SetUnorderedAccessView(m_Direct3D->GetDeviceContext());

    // s0
    m_Sampler->UseCSLinear(m_Direct3D->GetDeviceContext(), 0);

    // t0
    // LightInjectionShader 에서 UAV 로 쓰인 텍스쳐는 여기서 SRV 로 쓰임.
    m_TemporalIntegrationVoxelGrid[flag ? 1 : 0]->UseCSShaderResourceView(m_Direct3D->GetDeviceContext(), 0);
    
    size_x = static_cast<uint32_t>(ceil(float(m_ScreenWidth / 8) / float(LOCAL_SIZE_X)));
    size_y = static_cast<uint32_t>(ceil(float(m_ScreenHeight / 8) / float(LOCAL_SIZE_Y)));
    size_z = 1;

    // m_RaymarchShader 계산
    m_RaymarchShader->Compute(m_Direct3D->GetDeviceContext(), biasNearFarPow, anisoDensityScatteringAbsorption, size_x, size_y, size_z);


    ID3D11UnorderedAccessView* nulluav = nullptr;

    // 계산이 끝났으면 이 텍스쳐를 SRV 로 사용해야 하므로 UAV 상태를 해제해준다.
    m_Direct3D->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &nulluav, NULL);

    return true;
}


bool ApplicationClass::RenderVolumetricLight()
{
    XMMATRIX viewMatrix, projectionMatrix;

    float depthPower, screenNear, screenDepth;

    depthPower = 2.0f;
    screenNear = SCREEN_NEAR;
	screenDepth = SCREEN_DEPTH;

    // 카메라 및 d3d 객체에서 월드, 뷰 및 투영 행렬을 가져옵니다.
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);


    // 알파 블렌딩 켬
    m_Direct3D->EnableVolumetricFogAlphaDrawPassBlendingState();

    // 2D 렌더를 시작하려면 Z 버퍼를 꺼야한다.
    m_Direct3D->TurnZBufferOff();


    // s0
    m_Sampler->UsePSLinear(m_Direct3D->GetDeviceContext(), 0);

    // s1
    m_Sampler->UsePSPoint(m_Direct3D->GetDeviceContext(), 1);

    
    // 화면을 꽉 채우는 사각형 버텍스 세팅
    m_Direct3D->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    
	// Render 함수 내부에서 Matrix 들을 Transpose 해준 후 상수 버퍼에 써넣는다.
    m_VolumetricLightShader->Render(m_Direct3D->GetDeviceContext(),
        viewMatrix, projectionMatrix,
        XMMatrixInverse(nullptr, viewMatrix), XMMatrixInverse(nullptr, projectionMatrix),
        depthPower, screenNear, screenDepth, m_GBuffers->GetDepthResourceView(), m_RayMarchVoxelGrid->GetShaderResourceView());


    // 2D 렌더링이 완료되었으므로 Z 버퍼를 다시 킨다.
    m_Direct3D->TurnZBufferOn();
    
    // 알파 블렌딩 끔
    m_Direct3D->DisableAlphaBlending();




    return true;
}

