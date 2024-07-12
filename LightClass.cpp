#include "Stdafx.h"
#include "LightClass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}

bool LightClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	D3D11_BUFFER_DESC lightsBufferDesc;
	lightsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightsBufferDesc.ByteWidth = sizeof(LightProperties);
	lightsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightsBufferDesc.MiscFlags = 0;
	lightsBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&lightsBufferDesc, NULL, &m_lightsBuffer)))
	{
		return false;
	}

	D3D11_BUFFER_DESC shadowMapBufferDesc;
	shadowMapBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	shadowMapBufferDesc.ByteWidth = sizeof(ShadowMapProperties);
	shadowMapBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	shadowMapBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	shadowMapBufferDesc.MiscFlags = 0;
	shadowMapBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&shadowMapBufferDesc, NULL, &m_shadowMapBuffer)))
	{
		return false;
	}

	return true;
}

void LightClass::UpdateBuffer(ID3D11DeviceContext* deviceContext)
{
	deviceContext->UpdateSubresource(m_lightsBuffer, 0, nullptr, &m_lightProps, 0, 0);


	// 위쪽을 가리키는 벡터를 설정합니다.
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 lookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMVECTOR upVector = XMLoadFloat3(&up);


	XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);

	// 정사각형 광원에 대한 시야 및 화면 비율을 설정합니다.
	float fieldOfView = (float)XM_PI / 2.0f;
	float screenAspect = 1.0f;


	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 100.0f, 1.0f);


	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		XMVECTOR positionVector =
			XMLoadFloat3(&XMFLOAT3(
				m_lightProps.m_Lights[i].m_Position.x,
				m_lightProps.m_Lights[i].m_Position.y,
				m_lightProps.m_Lights[i].m_Position.z));

		m_shadowMapProps.m_ShadowMaps[i].m_lightViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
		m_shadowMapProps.m_ShadowMaps[i].m_lightProjectionMatrix = projectionMatrix;
	}

	deviceContext->UpdateSubresource(m_shadowMapBuffer, 0, nullptr, &m_shadowMapProps, 0, 0);
}

void LightClass::UseLightBuffer(ID3D11DeviceContext* deviceContext, int lightsBufferslot)
{
	// 라이트 버퍼는 픽셀 셰이더 b1 슬롯
	deviceContext->PSSetConstantBuffers(lightsBufferslot, 1, &m_lightsBuffer);
}


void LightClass::UseShaderMapBuffer(ID3D11DeviceContext* deviceContext, int ShadowMapBufferslot)
{
	// 셰도우 맵 버퍼는 픽셀 셰이더 b2 슬롯
	deviceContext->PSSetConstantBuffers(ShadowMapBufferslot, 2, &m_shadowMapBuffer);

	// 셰도우 맵 텍스쳐는 픽셀 셰이더 t4 슬롯
}


void LightClass::Shutdown()
{
	m_shadowMapBuffer->Release();
	m_shadowMapBuffer = 0;

	m_lightsBuffer->Release();
	m_lightsBuffer = 0;
}