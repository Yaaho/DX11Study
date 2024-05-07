#include "Stdafx.h"
#include "TextureClass.h"

TextureClass::TextureClass()
{
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	int width = 0;
	int height = 0;

	// targa �̹��� �����͸� �޸𸮿� �ε��Ѵ�.
	if (!LoadTarga(filename, height, width))
	{
		return false;
	}

	// �ؽ����� ����ü�� �����Ѵ�.
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// �� �ؽ��ĸ� �����Ѵ�.
	if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &m_texture)))
	{
		return false;
	}

	// targa �̹��� �������� �ʺ� ����� �����Ѵ�.
	UINT rowPitch = (width * 4) * sizeof(unsigned char);

	// targa �̹��� �����͸� �ؽ��Ŀ� �����Ѵ�.
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// ���̴� ���ҽ� �� ����ü�� �����Ѵ�.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// �ؽ����� ���̴� ���ҽ� �並 �����
	if (FAILED(device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView)))
	{
		return false;
	}

	// �� �ؽ��Ŀ� ���� �Ӹ��� �����Ѵ�
	deviceContext->GenerateMips(m_textureView);

	// �̹��� �����Ͱ� �ؽ��Ŀ� �ε� �Ǿ����Ƿ� targa �̹��� �����͸� �����Ѵ�.
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	// �ؽ��� �� ���Ҹ��� �����Ѵ�.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// �ؽ��ĸ� �����Ѵ�.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// targa �̹��� �����͸� �����Ѵ�.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}

bool TextureClass::LoadTarga(char* filename, int& height, int& width)
{
	// targa ������ ���̳ʸ� ���� ����.
	FILE* fileptr;
	if (fopen_s(&fileptr, filename, "rb") != 0)
	{
		return false;
	}

	// ���� ����� �о�´�.
	TargaHeader targeFileHeader;
	unsigned int count = (unsigned int)fread(&targeFileHeader, sizeof(TargaHeader), 1, fileptr);
	if (count != 1)
	{
		return false;
	}

	// ���� ������� �߿� ������ ���´�.
	height = (int)targeFileHeader.height;
	width = (int)targeFileHeader.width;
	int bpp = (int)targeFileHeader.bpp;

	// ������ 32bit ���� 24bit ���� üũ�Ѵ�.
	if (bpp != 32)
	{
		return false;
	}

	// 32��Ʈ �̹��� �������� ũ�⸦ ����Ѵ�.
	int imageSize = width * height * 4;

	// targa �̹��� �����Ϳ� �޸𸮸� �Ҵ��Ѵ�.
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	// targa �̹��� �����͸� �д´�.
	count = (unsigned int)fread(targaImage, 1, imageSize, fileptr);
	if (count != imageSize)
	{
		return false;
	}

	// ������ �ݴ´�.
	if (fclose(fileptr) != 0)
	{
		return false;
	}

	// targa ��� �����Ϳ� ���� �޸𸮸� �Ҵ��Ѵ�.
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	// targa ��� ������ �迭�� �ε����� �ʱ�ȭ�Ѵ�.
	int index = 0;

	// targa �̹��� �����Ϳ� �ε����� �ʱ�ȭ�Ѵ�.
	int k = (width * height * 4) - (width * 4);

	// ���� targa ������ �Ųٷ� ����Ǿ����Ƿ� �ùٸ� ������ targa �̹��� �����͸� targa ��� �迭�� �����Ѵ�.
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2]; // ����
			m_targaData[index + 1] = targaImage[k + 1]; // ���
			m_targaData[index + 2] = targaImage[k + 0]; // �Ķ�
			m_targaData[index + 3] = targaImage[k + 3]; // ����

			// �ε����� targa �����ͷ� ������Ų��.
			k += 4;
			index += 4;
		}

		// targa �̹��� ������ �ε����� �������� ���� �� ���� ���� �κп��� ���� ������ �ٽ� �����Ѵ�.
		k -= (width * 8);
	}

	// ��� �迭�� ���� �� targa �̹��� �����͸� �����Ѵ�.
	delete[] targaImage;
	targaImage = 0;

	return true;
}