#pragma once

#define MAX_LIGHTS 4

enum LightType
{
	DirectionalLight = 0,
	PointLight = 1,
	SpotLight = 2
};

enum LightStatus
{
	Disable = 0,
	Enable = 1,
	Static_Shadow = 2
};

struct Light
{
	XMFLOAT4 m_Position;
	XMFLOAT4 m_Direction;
	XMFLOAT4 m_Color;

	float m_CostantAttenuation = 1.0f;
	float m_LinearAttenuation = 0.09f;
	float m_QuadAttenuation = 0.032f;
	float m_SpotAngle;

	int m_LightType;
	int m_Status = LightStatus::Enable;
};

struct LightProperties
{
	XMFLOAT4 m_EyePosition;
	XMFLOAT4 m_GlobalAmbient;
	Light m_Lights[MAX_LIGHTS];
};

struct ShadowMapMatrix
{
	float m_ShadowMapTopLeftX;
	float m_ShadowMapTopLeftY;
	float m_ShadowMapTextureRatio;
	float padding;

	XMMATRIX m_lightViewMatrix;
	XMMATRIX m_lightProjectionMatrix;
};

struct ShadowMapProperties
{
	float m_MapWidth;
	float m_MapHeight;
	float padding1;
	float padding2;

	ShadowMapMatrix m_ShadowMaps[MAX_LIGHTS];
};


class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	bool Initialize(ID3D11Device*, HWND, float, float, float, float);
	void UpdateBuffer(ID3D11DeviceContext*);


	void UseLightBuffer(ID3D11DeviceContext*, int);
	void UseShadowMapBuffer(ID3D11DeviceContext*, int);


	void Shutdown();

	LightProperties m_lightProps;
	ShadowMapProperties m_shadowMapProps;

	ID3D11Buffer* m_lightsBuffer;
	ID3D11Buffer* m_shadowMapBuffer;

	float m_shadowMapWidth;
	float m_shadowMapHeight;

	float m_screenDepth;
	float m_screenNear;
};