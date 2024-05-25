#include "Stdafx.h"
#include "SpecMapShaderClass.h"

SpecMapShaderClass::SpecMapShaderClass()
{
}

SpecMapShaderClass::SpecMapShaderClass(const SpecMapShaderClass& other)
{
}

SpecMapShaderClass::~SpecMapShaderClass()
{
}

bool SpecMapShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// ���� �� �ȼ� ���̴��� �ʱ�ȭ�Ѵ�.
	return InitializeShader(device, hwnd, L"LightVS.HLSL", L"LightPS.HLSL");
}

void SpecMapShaderClass::Shutdown()
{
	// ���ؽ� �� �ȼ� ���̴��� ���õ� ��ü�� �����Ѵ�.
	ShutdownShader();
}

bool SpecMapShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView** textureArray, 
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 clipPlane, XMFLOAT4 specularColor, float specularPower)
{
	// �������� ����� ���̴� �Ű� ������ �����Ѵ�.
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray, 
		lightDirection, diffuseColor, cameraPosition, clipPlane, specularColor, specularPower))
	{
		return false;
	}

	// ������ ���۸� ���̴��� �������Ѵ�.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool SpecMapShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// ���ؽ� ���̴� �ڵ带 �������Ѵ�.
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, &vertexShaderBuffer, &errorMessage)))
	{
		// ���̴� ������ ���� �� ���� �޽����� ����Ѵ�.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����̴�.
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// �ȼ� ���̴� �ڵ带 �������Ѵ�.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, &pixelShaderBuffer, &errorMessage)))
	{
		// ���̴� ������ ���� �� ���� �޽����� ����Ѵ�.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����̴�.
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// ���ۿ��� ���� ���̴��� �����Ѵ�.
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		NULL, &m_vertexShader)))
	{
		return false;
	}

	// ���ۿ��� �ȼ� ���̴��� �����Ѵ�.
	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
		NULL, &m_pixelShader)))
	{
		return false;
	}


	// ���� �Է� ���̾ƿ� ����ü�� �����Ѵ�.
	// // �� ������ ModelClass �� ���̴��� VertexType ������ ��ġ�ؾ� �Ѵ�.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	// ���̾ƿ��� ��� ���� �����´�.
	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� �����.
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))
	{
		return false;
	}

	// �� �̻� ������ �ʴ� ���� ���̴� ���ۿ� �ȼ� ���̴� ���۸� �����Ѵ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���� ���̴��� �ִ� ��� ��� ������ ����ü�� �ۼ��Ѵ�.(����, ��, �������� ��Ʈ����)
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �Ѵ�.
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer)))
	{
		return false;
	}

	// ���ؽ� ���̴��� �ִ� ī�޶� ���� ��� ������ Description �� �����Ѵ�.
	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// �� Ŭ���� ������ ���ؽ� ���̴� ��� ���ۿ� ������ �� �� �ַ� ��� ���� �����͸� �����.
	if (FAILED(device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer)))
	{
		return false;
	}

	// ���ؽ� ���̴��� �ִ� ���� �Ȱ� ��� ������ Description �� �����Ѵ�.
	D3D11_BUFFER_DESC clipPlaneBufferDesc;
	clipPlaneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	clipPlaneBufferDesc.ByteWidth = sizeof(ClipPlaneBufferType);
	clipPlaneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	clipPlaneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	clipPlaneBufferDesc.MiscFlags = 0;
	clipPlaneBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �Ѵ�.
	if (FAILED(device->CreateBuffer(&clipPlaneBufferDesc, NULL, &m_ClipPlaneBuffer)))
	{
		return false;
	}

	// �ȼ� ���̴��� �ִ� ���� ���� ��� ������ Description �� �����Ѵ�.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// �� Ŭ���� ������ �ȼ� ���̴� ��� ���ۿ� ������ �� �� �ַ� ��� ���� �����͸� �����.
	if (FAILED(device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer)))
	{
		return false;
	}

	// �ؽ��� ���÷� ���� ����ü�� ���� �� �����Ѵ�.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(device->CreateSamplerState(&samplerDesc, &m_sampleState)))
	{
		return false;
	}

	return true;
}

void SpecMapShaderClass::ShutdownShader()
{
	// ���÷� ���¸� �����Ѵ�.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// ���� ��� ���۸� �����Ѵ�.
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	// �Ȱ� ��� ���۸� �����Ѵ�.
	if (m_ClipPlaneBuffer)
	{
		m_ClipPlaneBuffer->Release();
		m_ClipPlaneBuffer = 0;
	}

	// ī�޶� ��� ���۸� �����Ѵ�.
	if (m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = 0;
	}

	// ��� ��� ���۸� �����Ѵ�.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// ���̾ƿ��� �����Ѵ�.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// �ȼ� ���̴��� �����Ѵ�.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// ���ؽ� ���̴��� �����Ѵ�.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void SpecMapShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	// ���� �޽����� ���â�� ǥ���Ѵ�.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	// ���� �޽����� ��ȯ�Ѵ�.
	errorMessage->Release();
	errorMessage = 0;

	// ������ ������ ������ �˾� �޽����� �˷��ش�.
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}



bool SpecMapShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray,
	XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 clipPlane, XMFLOAT4 specularColor, float specularPower)
{
	// ����� transpose �Ͽ� ���̴����� ����� �� �ְ� �Ѵ�.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��ٴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����´�.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����Ѵ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ������ ����� Ǭ��.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����Ѵ�.
	unsigned bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲ۴�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);


	// camera constant buffer �� ����� �� �ֵ��� ��ٴ�.
	if (FAILED(deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// constant ������ �����Ϳ� ���� �����͸� �����´�.
	CameraBufferType* dataptr2 = (CameraBufferType*)mappedResource.pData;

	// ī�޶� ��ġ�� constant ���ۿ� �����Ѵ�.
	dataptr2->cameraPosition = cameraPosition;

	// constant ������ ����� �����Ѵ�.
	deviceContext->Unmap(m_cameraBuffer, 0);

	// ���ؽ� ���̴����� ī�޶� constant ������ ��ġ�� �����Ѵ�.
	bufferNumber = 1;

	// ���������� ������Ʈ �� ������ ���ؽ� ���̴����� ī�޶� ��� ���۸� �����Ѵ�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);

	// fog constant buffer �� ����� �� �ֵ��� ��ٴ�.
	if (FAILED(deviceContext->Map(m_ClipPlaneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	//const ������ �����Ϳ� ���� �����͸� �����´�.
	ClipPlaneBufferType* dataptr3 = (ClipPlaneBufferType*)mappedResource.pData;

	// �Ȱ� ������ constant ���ۿ� �����Ѵ�.
	dataptr3->clipPlane = clipPlane;
	
	// ��� ������ ����� �����Ѵ�.
	deviceContext->Unmap(m_ClipPlaneBuffer, 0);

	// ���� ���̴��� �Ȱ� ��� ������ ��ġ�� �����Ѵ�.
	bufferNumber = 2;

	// ������Ʈ �� ������ ���ؽ� ���̴����� �Ȱ� ���۸� �����Ѵ�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_ClipPlaneBuffer);

	// light constant buffer �� ����� �� �ֵ��� ��ٴ�
	if (FAILED(deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// constant ������ �����Ϳ� ���� �����͸� �����´�.
	LightBufferType* dataptr4 = (LightBufferType*)mappedResource.pData;

	// ���� ������ constant ���ۿ� �����Ѵ�.
	dataptr4->diffuseColor = diffuseColor;
	dataptr4->lightDirection = lightDirection;
	dataptr4->specularColor = specularColor;
	dataptr4->specularPower = specularPower;

	// constant ������ ����� �����Ѵ�.
	deviceContext->Unmap(m_lightBuffer, 0);

	// �ȼ� ���̴����� ���� constant ������ ��ġ�� �����Ѵ�.
	bufferNumber = 0;

	// ���������� ������Ʈ �� ������ �ȼ� ���̴����� ���� ��� ���۸� �����Ѵ�.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	// �ȼ� ���̴����� ���̴� �ؽ��� ���ҽ��� �����Ѵ�.
	deviceContext->PSSetShaderResources(0, 3, textureArray);

	return true;
}

void SpecMapShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ȼ� ���̴����� ���÷� ���¸� �����Ѵ�.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// �ﰢ���� �׸���.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
