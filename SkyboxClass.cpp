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
	// 버텍스 및 인덱스 버퍼를 종료합니다.
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

	// 정점 배열을 만든다.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
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


	// 정적 정점 버퍼의 구조체를 설정한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}


	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// 이제 버텍스와 인덱스 버퍼가 생성되고로드 된 배열을 해제하십시오.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void SkyboxClass::ShutdownBuffers()
{
	// 인덱스 버퍼를 해제합니다.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// 버텍스 버퍼를 해제한다.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void SkyboxClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 꼭지점 버퍼에서 렌더링되어야하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}