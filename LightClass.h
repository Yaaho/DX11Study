#pragma once

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

    void SetAmbientColor(float, float, float, float);
    void SetDiffuseColor(float, float, float, float);
    void SetPosition(float, float, float);
    void SetLookAt(float, float, float);

    XMFLOAT4 GetAmbientColor();
    XMFLOAT4 GetDiffuseColor();
    XMFLOAT3 GetPosition();

    void GenerateViewMatrix();
    void GetViewMatrix(XMMATRIX&);

    void GenerateOrthoMatrix(float, float, float);
    void GetOrthoMatrix(XMMATRIX&);

    void SetDirection(float, float, float);
    XMFLOAT3 GetDirection();

private:
    XMFLOAT4 m_ambientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    XMFLOAT4 m_diffuseColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3 m_lookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMMATRIX m_viewMatrix;
    XMMATRIX m_orthoMatrix;
    XMFLOAT3 m_direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
};