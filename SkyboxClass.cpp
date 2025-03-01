#include "Stdafx.h"
#include "SkyboxClass.h"

SkyboxClass::SkyboxClass()
{

}

SkyboxClass::SkyboxClass(const SkyboxClass& other)
{

}

SkyboxClass::~SkyboxClass()
{
}

bool SkyboxClass::Initialize(ID3D11Device* device)
{
	return InitializeBuffers(device);
}

void SkyboxClass::Shutdown()
{
	// ���ؽ� �� �ε��� ���۸� �����մϴ�.
	ShutdownBuffers();
}

void SkyboxClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int SkyboxClass::GetIndexCount()
{
	return m_indexCount;
}

bool SkyboxClass::InitializeBuffers(ID3D11Device* device)
{
	m_vertexCount = 24;
	m_indexCount = 36;

	// ���� �迭�� �����.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// �ε��� �迭�� ����ϴ�.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	vertices[1].position = XMFLOAT3(1.0f, -1.0f, -1.0f);
	vertices[2].position = XMFLOAT3(1.0f, 1.0f, -1.0f);
	vertices[3].position = XMFLOAT3(-1.0f, 1.0f, -1.0f);

	vertices[4].position = XMFLOAT3(1.0f, -1.0f, 1.0f);
	vertices[5].position = XMFLOAT3(-1.0f, -1.0f, 1.0f);
	vertices[6].position = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	vertices[7].position = XMFLOAT3(1.0f, 1.0f, 1.0f);

	vertices[8].position = XMFLOAT3(-1.0f, -1.0f, 1.0f);
	vertices[9].position = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	vertices[10].position = XMFLOAT3(-1.0f, 1.0f, -1.0f);
	vertices[11].position = XMFLOAT3(-1.0f, 1.0f, 1.0f);

	vertices[12].position = XMFLOAT3(1.0f, -1.0f, -1.0f);
	vertices[13].position = XMFLOAT3(1.0f, -1.0f, 1.0f);
	vertices[14].position = XMFLOAT3(1.0f, 1.0f, 1.0f);
	vertices[15].position = XMFLOAT3(1.0f, 1.0f, -1.0f);

	vertices[16].position = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	vertices[17].position = XMFLOAT3(-1.0f, -1.0f, 1.0f);
	vertices[18].position = XMFLOAT3(1.0f, -1.0f, 1.0f);
	vertices[19].position = XMFLOAT3(1.0f, -1.0f, -1.0f);

	vertices[20].position = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	vertices[21].position = XMFLOAT3(-1.0f, 1.0f, -1.0f);
	vertices[22].position = XMFLOAT3(1.0f, 1.0f, -1.0f);
	vertices[23].position = XMFLOAT3(1.0f, 1.0f, 1.0f);


	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	indices[6] = 4;
	indices[7] = 5;
	indices[8] = 6;
	indices[9] = 4;
	indices[10] = 6;
	indices[11] = 7;

	indices[12] = 8;
	indices[13] = 9;
	indices[14] = 10;
	indices[15] = 8;
	indices[16] = 10;
	indices[17] = 11;

	indices[18] = 12;
	indices[19] = 13;
	indices[20] = 14;
	indices[21] = 12;
	indices[22] = 14;
	indices[23] = 15;

	indices[24] = 16;
	indices[25] = 17;
	indices[26] = 18;
	indices[27] = 16;
	indices[28] = 18;
	indices[29] = 19;

	indices[30] = 20;
	indices[31] = 21;
	indices[32] = 22;
	indices[33] = 20;
	indices[34] = 22;
	indices[35] = 23;


	// ���� ���� ������ ����ü�� �����Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ���۸� ����ϴ�.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}


	// ���� �ε��� ������ ����ü�� �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� ����ϴ�.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// ���� ���ؽ��� �ε��� ���۰� �����ǰ��ε� �� �迭�� �����Ͻʽÿ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void SkyboxClass::ShutdownBuffers()
{
	// �ε��� ���۸� �����մϴ�.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// ���ؽ� ���۸� �����Ѵ�.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void SkyboxClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ����������� ���� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ����������� �ε��� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �� ������ ���ۿ��� �������Ǿ���ϴ� ������Ƽ�� ������ �����մϴ�.�� ��쿡�� �ﰢ���Դϴ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}