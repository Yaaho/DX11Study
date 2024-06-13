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
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// ������ ����Ű�� ���͸� �����մϴ�.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	upVector = XMLoadFloat3(&up);

	// 3D���忡�� ī�޶��� ��ġ�� ??�����մϴ�.
	position = m_position;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	positionVector = XMLoadFloat3(&position);

	// �⺻������ ī�޶� ã���ִ� ��ġ�� �����մϴ�.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	lookAtVector = XMLoadFloat3(&lookAt);

	// yaw (Y ��), pitch (X ��) �� roll (Z ��)�� ȸ������ ���� ������ �����մϴ�.
	pitch = m_rotation.x * 0.0174532925f;
	yaw = m_rotation.y * 0.0174532925f;
	roll = m_rotation.z * 0.0174532925f;

	//  yaw, pitch, roll ���� ���� ȸ�� ����� ����ϴ�.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// lookAt �� up ���͸� ȸ�� ��ķ� �����Ͽ� �䰡 �������� �ùٸ��� ȸ���ǵ��� �մϴ�.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// ȸ�� �� ī�޶� ��ġ�� ��� ��ġ�� ��ȯ�մϴ�.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// ���������� �� ���� ������Ʈ �� ���Ϳ��� �� ����� ����ϴ�.
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
	// ������ ����Ű�� ���͸� �����մϴ�.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	upVector = XMLoadFloat3(&up);

	// 3D���忡�� ī�޶��� ��ġ�� ??�����մϴ�.
	position = m_position;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	positionVector = XMLoadFloat3(&position);

	// ���� ������ ȸ���� ����մϴ�.
	float radians = m_rotation.y * 0.0174532925f;

	// ī�޶� �������� lookAt ���� �����մϴ�.
	lookAt.x = sinf(radians) + m_position.x;
	lookAt.y = m_position.y;
	lookAt.z = cosf(radians) + m_position.z;

	// XMVECTOR ����ü�� �ε��Ѵ�.
	lookAtVector = XMLoadFloat3(&lookAt);

	// ���������� �� ���� ������Ʈ �� ���Ϳ��� �� ����� ����ϴ�.
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