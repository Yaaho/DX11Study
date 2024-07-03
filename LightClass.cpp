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

bool LightClass::CreateBuffer(ID3D11Device* device)
{
	D3D11_BUFFER_DESC lightsBufferDesc;
	lightsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightsBufferDesc.ByteWidth = sizeof(LightProperties);
	lightsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightsBufferDesc.MiscFlags = 0;
	lightsBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&lightsBufferDesc, NULL, &m_lightsBuffer)))
	{
		return false;
	}

	return true;
}

void LightClass::UpdateBuffer(ID3D11DeviceContext* deviceContext)
{
	deviceContext->UpdateSubresource(m_lightsBuffer, 0, nullptr, &m_lightProps, 0, 0);
}

void LightClass::Use(ID3D11DeviceContext* deviceContext, int slot)
{
	deviceContext->PSSetConstantBuffers(slot, 1, &m_lightsBuffer);
}

void LightClass::Shutdown()
{
	m_lightsBuffer->Release();
	m_lightsBuffer = 0;
}