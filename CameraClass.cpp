#include "Stdafx.h"
#include "CameraClass.h"

CameraClass::CameraClass()
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

CameraClass::CameraClass(const CameraClass& other)
{

}

CameraClass::~CameraClass()
{

}

void CameraClass::SetPosition(XMFLOAT3 p)
{
	m_position = p;
}


void CameraClass::SetRotation(XMFLOAT3 p)
{
	m_rotation = p;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return m_position;
}

XMFLOAT3 CameraClass::GetRotation()
{
	return m_rotation;
}

void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;

	// ������ ����Ű�� ���͸� �����Ѵ�.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	XMVECTOR upVector = XMLoadFloat3(&up);

	// 3D ���忡�� ī�޶��� ��ġ�� �����Ѵ�.
	position = m_position;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	XMVECTOR positionVector = XMLoadFloat3(&position);

	// rotation �� �������� ����Ѵ�.
	float radians = m_rotation.y * 0.0174532925f;

	// �⺻������ ī�޶� �����ִ� ��ġ�� �����Ѵ�.
	lookAt.x = sinf(radians) + m_position.x;
	lookAt.y = m_position.y;
	lookAt.z = cosf(radians) + m_position.z;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);
	
	// �� ���� ������Ʈ �� ���Ϳ��� �� ����� �����.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}


void CameraClass::RenderBaseViewMatrix()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVector = XMLoadFloat3(&up);
	position = m_position;
	positionVector = XMLoadFloat3(&position);
	float radians = m_rotation.y * 0.0174532925f;

	lookAt.x = sinf(radians) + m_position.x;
	lookAt.y = m_position.y;
	lookAt.z = cosf(radians) + m_position.z;

	lookAtVector = XMLoadFloat3(&lookAt);
	m_baseViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetBaseViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_baseViewMatrix;
}

void CameraClass::RenderReflection(float height)
{
	XMFLOAT3 up, position, lookAt;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	XMVECTOR upVector = XMLoadFloat3(&up);

	position.x = m_position.x;
	position.y = -m_position.y + (height * 2.0f);
	position.z = m_position.z;

	XMVECTOR positionVector = XMLoadFloat3(&position);

	float radians = m_rotation.y * 0.0174532925f;

	lookAt.x = sinf(radians) + m_position.x;
	lookAt.y = position.y;
	lookAt.z = cosf(radians) + m_position.z;

	XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);

	m_reflectionViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}


XMMATRIX CameraClass::GetReflectionViewMatrix()
{
	return m_reflectionViewMatrix;
}