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

	// 위쪽을 가리키는 벡터를 설정한다.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// XMVECTOR 구조체에 로드한다.
	XMVECTOR upVector = XMLoadFloat3(&up);

	// 3D 월드에서 카메라의 위치를 설정한다.
	position = m_position;

	// XMVECTOR 구조체에 로드한다.
	XMVECTOR positionVector = XMLoadFloat3(&position);

	// rotation 을 라디안으로 계산한다.
	float radians = m_rotation.y * 0.0174532925f;

	// 기본적으로 카메라가 보고있는 위치를 설정한다.
	lookAt.x = sinf(radians) + m_position.x;
	lookAt.y = m_position.y;
	lookAt.z = cosf(radians) + m_position.z;

	// XMVECTOR 구조체에 로드한다.
	XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);
	
	// 세 개의 업데이트 된 벡터에서 뷰 행렬을 만든다.
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