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

bool LightClass::Initialize(ID3D11Device* device, HWND hwnd, float shadowMapWidth, float shadowMapHeight, 
	float screenDepth, float screenNear)
{
	D3D11_BUFFER_DESC lightsBufferDesc;
	// UpdateSubresource �� ����ϱ� ���ؼ� Usage �� DEFAULT �� ����
	lightsBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	lightsBufferDesc.ByteWidth = sizeof(LightProperties);
	lightsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	// Usage �� DEFAULT �� ��� CPUAccessFlags �� 0 �̾�� ��
	lightsBufferDesc.CPUAccessFlags = 0;
	lightsBufferDesc.MiscFlags = 0;
	lightsBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&lightsBufferDesc, NULL, &m_lightsBuffer)))
	{
		return false;
	}

	D3D11_BUFFER_DESC shadowMapBufferDesc;
	// UpdateSubresource �� ����ϱ� ���ؼ� Usage �� DEFAULT �� ����
	shadowMapBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	shadowMapBufferDesc.ByteWidth = sizeof(ShadowMapProperties);
	shadowMapBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	// Usage �� DEFAULT �� ��� CPUAccessFlags �� 0 �̾�� ��
	shadowMapBufferDesc.CPUAccessFlags = 0;
	shadowMapBufferDesc.MiscFlags = 0;
	shadowMapBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&shadowMapBufferDesc, NULL, &m_shadowMapBuffer)))
	{
		return false;
	}

	m_shadowMapWidth = shadowMapWidth;
	m_shadowMapHeight = shadowMapHeight;

	m_screenDepth = screenDepth;
	m_screenNear = screenNear;

	return true;
}

void LightClass::UpdateBuffer(ID3D11DeviceContext* deviceContext)
{
	// ������ ����Ű�� ���͸� �����մϴ�.
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 lookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMVECTOR upVector = XMLoadFloat3(&up);


	XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);

	// ���簢�� ������ ���� �þ� �� ȭ�� ������ �����մϴ�.
	float fieldOfView = (float)XM_PI / 2.0f;
	float screenAspect = 1.0f;

	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, m_screenNear, m_screenDepth);


	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		XMVECTOR positionVector =
			XMLoadFloat3(&XMFLOAT3(
				m_lightProps.m_Lights[i].m_Position.x,
				m_lightProps.m_Lights[i].m_Position.y,
				m_lightProps.m_Lights[i].m_Position.z));

		m_shadowMapProps.m_MapWidth = m_shadowMapWidth;
		m_shadowMapProps.m_MapHeight = m_shadowMapHeight;

		m_shadowMapProps.m_ShadowMaps[i].m_lightViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
		m_shadowMapProps.m_ShadowMaps[i].m_lightProjectionMatrix = projectionMatrix;
	}


	deviceContext->UpdateSubresource(m_lightsBuffer, 0, nullptr, &m_lightProps, 0, 0);
	deviceContext->UpdateSubresource(m_shadowMapBuffer, 0, nullptr, &m_shadowMapProps, 0, 0);

}

void LightClass::UseLightBuffer(ID3D11DeviceContext* deviceContext, int lightsBufferslot)
{
	// ����Ʈ ���۴� �ȼ� ���̴� b1 ����
	deviceContext->PSSetConstantBuffers(lightsBufferslot, 1, &m_lightsBuffer);
}


void LightClass::UseShadowMapBuffer(ID3D11DeviceContext* deviceContext, int ShadowMapBufferslot)
{
	// �ε��� �� ���۴� �ȼ� ���̴� b2 ����
	deviceContext->PSSetConstantBuffers(ShadowMapBufferslot, 1, &m_shadowMapBuffer);
}


void LightClass::Shutdown()
{
	m_shadowMapBuffer->Release();
	m_shadowMapBuffer = 0;

	m_lightsBuffer->Release();
	m_lightsBuffer = 0;
}