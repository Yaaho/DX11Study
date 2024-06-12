#pragma once

class TextureArrayClass;

class ModelClass : public AlignedAllocationPolicy<16>
{
private:
	struct  VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT4 color;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	struct InstanceType
	{
		XMFLOAT3 position;
	};
	
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, char*);
	bool LoadTextures(ID3D11Device*, WCHAR*);
	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetVertexCount();
	int GetIndexCount();
	int GetInstanceCount();
	ID3D11ShaderResourceView* GetTexture(int index);
	ID3D11ShaderResourceView** GetTextureArray();

	void SetPosition(float, float, float);
	void GetPosition(float&, float&, float&);

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	void ReleaseTexture();

	bool LoadModel(char*);
	void ReleaseModel();

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	ID3D11Buffer* m_instanceBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	int m_instanceCount = 0;

	ModelType* m_model = nullptr;
	XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);

	TextureArrayClass* m_TextureArray = nullptr;
};