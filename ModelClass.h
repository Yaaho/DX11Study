#pragma once

class TextureArrayClass;

class ModelClass : public AlignedAllocationPolicy<16>
{
private:
	struct  VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texCoord;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
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

	bool Initialize(ID3D11Device*, char*, float);
	bool LoadTextures(ID3D11Device*, const WCHAR*);
	bool LoadTextures(ID3D11Device*, const WCHAR*, const WCHAR*);
	bool LoadTextures(ID3D11Device*, const WCHAR*, const WCHAR*, const WCHAR*);
	bool LoadTextures(ID3D11Device*, const WCHAR*, const WCHAR*, const WCHAR*, const WCHAR*);
	bool LoadTextures(ID3D11Device*, const WCHAR*, const WCHAR*, const WCHAR*, const WCHAR*, const WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetVertexCount();
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture(int index);
	ID3D11ShaderResourceView** GetTextureArray();

	void SetPosition(XMFLOAT3);
	void GetPosition(XMFLOAT3&);

private:
	bool InitializeBuffers(ID3D11Device*, float);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	void ReleaseTexture();

	bool LoadModel(const char*);
	void ReleaseModel();

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

private:
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	int m_vertexCount = 0;
	int m_indexCount = 0;

	ModelType* m_model = nullptr;
	XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);

	TextureArrayClass* m_TextureArray = nullptr;
};