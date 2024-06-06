#include "Stdafx.h"
#include "PositionClass.h"

PositionClass::PositionClass()
{
}

PositionClass::PositionClass(const PositionClass& other)
{
}

PositionClass::~PositionClass()
{
}

void PositionClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
}


void PositionClass::SetRotation(float x, float y, float z)
{
	m_rotation = XMFLOAT3(x, y, z);
}


void PositionClass::GetPosition(XMFLOAT3& p)
{
	p = m_position;
}


void PositionClass::GetRotation(XMFLOAT3& p)
{
	p = m_rotation;
}

void PositionClass::SetFrameTime(float time)
{
	m_frameTime = time;
}


void PositionClass::MoveLeft(bool keydown)
{
	// 키를 누르면 카메라가 왼쪽으로 돌아는 속도가 증가합니다. 회전 속도를 늦추지 않으면.
	if (keydown)
	{
		m_leftSpeed += m_frameTime * 0.001f;

		if (m_leftSpeed > (m_frameTime * 0.03f))
		{
			m_leftSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		m_leftSpeed -= m_frameTime * 0.0007f;

		if (m_leftSpeed < 0.0f)
		{
			m_leftSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	float radians = m_rotation.y * 0.0174532925f;

	// Update the position.
	m_position.x -= cosf(radians) * m_leftSpeed;
	m_position.z -= sinf(radians) * m_leftSpeed;
}


void PositionClass::MoveRight(bool keydown)
{
	// 키를 누르면 카메라가 오른쪽으로 회전하는 속도가 증가합니다. 회전 속도를 늦추지 않으면.
	if (keydown)
	{
		m_rightSpeed += m_frameTime * 0.001f;

		if (m_rightSpeed > (m_frameTime * 0.03f))
		{
			m_rightSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		m_rightSpeed -= m_frameTime * 0.0007f;

		if (m_rightSpeed < 0.0f)
		{
			m_rightSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	float radians = m_rotation.y * 0.0174532925f;

	// Update the position.
	m_position.x += cosf(radians) * m_rightSpeed;
	m_position.z += sinf(radians) * m_rightSpeed;
}


void PositionClass::TurnLeft(bool Keydown)
{
	// 키를 누르면 카메라가 왼쪽으로 돌아가는 속도가 증가한다.
	if (Keydown)
	{
		m_leftTurnSpeed += m_frameTime * 0.01f;

		if (m_leftTurnSpeed > (m_frameTime * 0.15f))
		{
			m_leftTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime * 0.005f;

		if (m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}

	// 회전 속도를 사용하여 회전을 업데이트한다.
	m_rotation.y -= m_leftTurnSpeed;
	if (m_rotation.y < 0.0f)
	{
		m_rotation.y += 360.0f;
	}
}

void PositionClass::TurnRight(bool keydown)
{
	// 키를 누르면 카메라가 오른쪽으로 회전하는 속도가 증가한다.
	if (keydown)
	{
		m_rightTurnSpeed += m_frameTime * 0.01f;

		if (m_rightTurnSpeed > (m_frameTime * 0.15f))
		{
			m_rightTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime * 0.005f;

		if (m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}

	// 회전 속도를 사용하여 회전을 업데이트한다.
	m_rotation.y += m_rightTurnSpeed;
	if (m_rotation.y > 360.0f)
	{
		m_rotation.y -= 360.0f;
	}
}