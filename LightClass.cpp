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

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
    m_ambientColor = XMFLOAT4(red, green, blue, alpha);
}


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
    m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetPosition(float x, float y, float z)
{
    m_position = XMFLOAT3(x, y, z);
}


void LightClass::SetLookAt(float x, float y, float z)
{
    m_lookAt = XMFLOAT3(x, y, z);
}


XMFLOAT4 LightClass::GetAmbientColor()
{
    return m_ambientColor;
}


XMFLOAT4 LightClass::GetDiffuseColor()
{
    return m_diffuseColor;
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


void LightClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
    viewMatrix = m_viewMatrix;
}


void LightClass::GenerateOrthoMatrix(float width, float depthPlane, float nearPlane)
{
    // ���� ���� ����� ����ϴ�.
    m_orthoMatrix = XMMatrixOrthographicLH(width, width, nearPlane, depthPlane);
}


void LightClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
    orthoMatrix = m_orthoMatrix;
}


void LightClass::SetDirection(float x, float y, float z)
{
    m_direction = XMFLOAT3(x, y, z);
}


XMFLOAT3 LightClass::GetDirection()
{
    return m_direction;
}
