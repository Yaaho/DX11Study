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

void CameraClass::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
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

	// �⺻������ ī�޶� ã���ִ� ��ġ�� �����Ѵ�.
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
