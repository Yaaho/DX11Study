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
	XMFLOAT3 up, position, lookAt, debuglookAt;

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

	// 기본적으로 카메라가 찾고있는 위치를 설정한다.
	lookAt.x = sinf(radians) + m_position.x;
	lookAt.y = m_position.y;
	lookAt.z = cosf(radians) + m_position.z;

	debuglookAt.x = 0.0f;
	debuglookAt.y = 1.0f;
	debuglookAt.z = 0.0f;

	// XMVECTOR 구조체에 로드한다.
	XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);
	XMVECTOR debuglookAtVector = XMLoadFloat3(&debuglookAt);

	// 세 개의 업데이트 된 벡터에서 뷰 행렬을 만든다.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
	m_debugviewMatrix = XMMatrixLookAtLH(positionVector, debuglookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void CameraClass::GetDebugViewMatrix(XMMATRIX& debugviewMatrix)
{
	debugviewMatrix = m_debugviewMatrix;
}