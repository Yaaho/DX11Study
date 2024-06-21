#include "Stdafx.h"
#include "TextureClass.h"
#include "TreeClass.h"

#include <fstream>
using namespace std;


TreeClass::TreeClass()
{
}


TreeClass::TreeClass(const TreeClass& other)
{
}


TreeClass::~TreeClass()
{
}


bool TreeClass::Initialize(ID3D11Device* device, const char* trunkModelFilename, const WCHAR* trunkTextureFilename,
	const char* leafModelFilename, const WCHAR* leafTextureFilename, float scale)
{
	// ���� �� Ʈ��ũ �����͸� �ε��մϴ�.
	bool result = LoadModel(trunkModelFilename);
	if (!result)
	{
		return false;
	}

	// Ʈ��ũ �ε��� ���� �����Ͻʽÿ�.
	m_trunkIndexCount = m_indexCount;

	// Ʈ�� Ʈ��ũ�� ���� ������ �����ϰ� �ִ� ������ �ε��� ���۸� �ʱ�ȭ�մϴ�.
	result = InitializeTrunkBuffers(device, scale);
	if (!result)
	{
		return false;
	}

	// ���ۿ� �ε�� Ʈ�� Ʈ��ũ �� �����͸� �����մϴ�.
	ReleaseModel();

	// Ʈ�� ���� �� �����͸� �ε��մϴ�.
	result = LoadModel(leafModelFilename);
	if (!result)
	{
		return false;
	}

	// ���� �ε��� ���� �����մϴ�.
	m_leafIndexCount = m_indexCount;

	// Ʈ�� �����ٿ� ���� ���� ��Ʈ���� �����ϴ� ���� �� �ε��� ���۸� �ʱ�ȭ�մϴ�.
	result = InitializeLeafBuffers(device, scale);
	if (!result)
	{
		return false;
	}

	// Ʈ�� ���� �� �����͸� �����մϴ�.
	ReleaseModel();

	// Ʈ�� ���� �ؽ�ó�� �ε��մϴ�.
	result = LoadTextures(device, trunkTextureFilename, leafTextureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}


void TreeClass::Shutdown()
{
	// �ؽ�ó�� �����Ѵ�.
	ReleaseTextures();

	// ���ؽ��� �ε��� ���۸� �����Ѵ�.
	ShutdownBuffers();

	// ���� ������ ���� ���� �� �����͸� �����մϴ�.
	ReleaseModel();
}


void TreeClass::RenderTrunk(ID3D11DeviceContext* deviceContext)
{
	// �׸��⸦ �غ��ϱ� ���� �׷��� ������ ���ο� �������� �ε��� ���۸� �����ϴ�.
	RenderTrunkBuffers(deviceContext);
}


void TreeClass::RenderLeaves(ID3D11DeviceContext* deviceContext)
{
	// �׸��⸦ �غ��ϱ� ���� �׷��� ������ ���ο� �������� �ε��� ���۸� �����ϴ�.
	RenderLeafBuffers(deviceContext);
}


int TreeClass::GetTrunkIndexCount()
{
	return m_trunkIndexCount;
}


int TreeClass::GetLeafIndexCount()
{
	return m_leafIndexCount;
}


ID3D11ShaderResourceView* TreeClass::GetTrunkTexture()
{
	return m_TrunkTexture->GetTexture();
}


ID3D11ShaderResourceView* TreeClass::GetLeafTexture()
{
	return m_LeafTexture->GetTexture();
}


bool TreeClass::InitializeTrunkBuffers(ID3D11Device* device, float scale)
{
	// ���� �迭�� ����ϴ�.
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

	// ���� �迭�� �ε��� �迭�� �����ͷ� �ε��մϴ�.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x * scale, m_model[i].y * scale, m_model[i].z * scale);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// ���� ���� ������ ����ü�� �����մϴ�.
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
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_trunkVertexBuffer)))
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
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_trunkIndexBuffer)))
	{
		return false;
	}

	// ���� ���ؽ��� �ε��� ���۰� �����ǰ� �ε�� �迭�� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


bool TreeClass::InitializeLeafBuffers(ID3D11Device* device, float scale)
{
	// ���� �迭�� ����ϴ�.
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

	// ���� �迭�� �ε��� �迭�� �����ͷ� �ε��մϴ�.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x * scale, m_model[i].y * scale, m_model[i].z * scale);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// ���� ���� ������ ����ü�� �����մϴ�.
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
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_leafVertexBuffer)))
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
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_leafIndexBuffer)))
	{
		return false;
	}

	// ���� ���ؽ��� �ε��� ���۰� �����ǰ� �ε�� �迭�� �����մϴ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void TreeClass::ShutdownBuffers()
{
	// ���۸� �����մϴ�.
	if (m_leafIndexBuffer)
	{
		m_leafIndexBuffer->Release();
		m_leafIndexBuffer = 0;
	}

	if (m_leafVertexBuffer)
	{
		m_leafVertexBuffer->Release();
		m_leafVertexBuffer = 0;
	}

	if (m_trunkIndexBuffer)
	{
		m_trunkIndexBuffer->Release();
		m_trunkIndexBuffer = 0;
	}

	if (m_trunkVertexBuffer)
	{
		m_trunkVertexBuffer->Release();
		m_trunkVertexBuffer = 0;
	}
}


void TreeClass::RenderTrunkBuffers(ID3D11DeviceContext* deviceContext)
{
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_trunkVertexBuffer, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetIndexBuffer(m_trunkIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//�� ������ ���ۿ��� �������Ǿ�� �ϴ� ������Ƽ�� ������ �����մϴ�.�� ��쿡�� �ﰢ���Դϴ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void TreeClass::RenderLeafBuffers(ID3D11DeviceContext* deviceContext)
{
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_leafVertexBuffer, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	deviceContext->IASetIndexBuffer(m_leafIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//�� ������ ���ۿ��� �������Ǿ���ϴ� ������Ƽ�� ������ �����մϴ�.�� ��쿡�� �ﰢ���Դϴ�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool TreeClass::LoadTextures(ID3D11Device* device, const WCHAR* trunkFilename, const WCHAR* leafFilename)
{
	// Ʈ��ũ �ؽ�ó ��ü�� ����ϴ�.
	m_TrunkTexture = new TextureClass;
	if (!m_TrunkTexture)
	{
		return false;
	}

	// Ʈ��ũ �ؽ�ó ��ü�� �ʱ�ȭ �մϴ�.
	if (!m_TrunkTexture->Initialize(device, trunkFilename))
	{
		return false;
	}

	// ���� �ؽ�ó ��ü�� ����ϴ�.
	m_LeafTexture = new TextureClass;
	if (!m_LeafTexture)
	{
		return false;
	}

	// ���� �ؽ�ó ��ü�� �ʱ�ȭ �մϴ�.
	if (!m_LeafTexture->Initialize(device, leafFilename))
	{
		return false;
	}

	return true;
}


void TreeClass::ReleaseTextures()
{
	// �ؽ�ó ������Ʈ�� �������Ѵ�.
	if (m_LeafTexture)
	{
		m_LeafTexture->Shutdown();
		delete m_LeafTexture;
		m_LeafTexture = 0;
	}

	if (m_TrunkTexture)
	{
		m_TrunkTexture->Shutdown();
		delete m_TrunkTexture;
		m_TrunkTexture = 0;
	}
}


bool TreeClass::LoadModel(const char* filename)
{
	// �� ������ ���ϴ�.
	ifstream fin;
	fin.open(filename);

	// ������ ���� ������ �����մϴ�.
	if (fin.fail())
	{
		return false;
	}

	// ���ؽ� ī��Ʈ�� ������ �д´�.
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// ���ؽ� ī��Ʈ�� �д´�.
	fin >> m_vertexCount;

	// �ε����� ���� �������� ���� �����մϴ�.
	m_indexCount = m_vertexCount;

	// �о� ���� ���� ������ ����Ͽ� ���� ����ϴ�.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// �������� ���� �κб��� �д´�.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// ���ؽ� �����͸� �н��ϴ�.
	for (int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// �� ������ �ݴ´�.
	fin.close();

	return true;
}


void TreeClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}
}


void TreeClass::SetPosition(XMFLOAT3 pos)
{
	m_position = pos;
}

void TreeClass::GetPosition(XMFLOAT3& pos)
{
	pos = m_position;
}