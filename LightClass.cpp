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


void LightClass::SetDirection(XMFLOAT3 direction)
{
	m_direction = direction;
}

void LightClass::SetPosition(XMFLOAT3 position)
{
	m_position = position;
}


void LightClass::SetLookAt(XMFLOAT3 lookat)
{
	m_lookAt = lookat;
}


XMFLOAT3 LightClass::GetDirection()
{
	return m_direction;
}


XMFLOAT3 LightClass::GetPosition()
{
	return m_position;
}


void LightClass::GenerateViewMatrix()
{
	// ������ ����Ű�� ���͸� �����մϴ�.
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	XMVECTOR upVector = XMLoadFloat3(&up);
	XMVECTOR positionVector = XMLoadFloat3(&m_position);
	XMVECTOR lookAtVector = XMLoadFloat3(&m_lookAt);

	// �� ���ͷκ��� �� ����� ����ϴ�.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}


void LightClass::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	// ���簢�� ������ ���� �þ� �� ȭ�� ������ �����մϴ�.
	float fieldOfView = (float)XM_PI / 2.0f;
	float screenAspect = 1.0f;

	// ���� ���� ����� ����ϴ�.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
}


void LightClass::GenerateOrthoMatrix(float width, float height, float screenDepth, float screenNear)
{
	// ���� ���� ���� ����� ����ϴ�.
	m_orthoMatrix = XMMatrixOrthographicLH(width, height, screenNear, screenDepth);
}


void LightClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}


void LightClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}


void LightClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}