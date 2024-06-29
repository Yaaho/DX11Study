#include "Stdafx.h"
#include "TextureArrayClass.h"

TextureArrayClass::TextureArrayClass()
{
}


TextureArrayClass::TextureArrayClass(const TextureArrayClass& other)
{
}

TextureArrayClass::~TextureArrayClass()
{
}


bool TextureArrayClass::Initialize(ID3D11Device* device, const WCHAR* filename1)
{
	// ù��° �ؽ��ĸ� ���Ϸκ��� �о�´�
	if (FAILED(CreateDDSTextureFromFile(device, filename1, nullptr, &m_texture[0])))
	{
		return false;
	}

	textureCount = 1;

	return true;
}

bool TextureArrayClass::Initialize(ID3D11Device* device, const WCHAR* filename1, const WCHAR* filename2)
{

	// ù��° �ؽ��ĸ� ���Ϸκ��� �о�´�
	if (FAILED(CreateDDSTextureFromFile(device, filename1, nullptr, &m_texture[0])))
	{
		return false;
	}

	// �ι�° �ؽ��ĸ� ���Ϸκ��� �о�´�.
	if (FAILED(CreateDDSTextureFromFile(device, filename2, nullptr, &m_texture[1])))
	{
		return false;
	}

	textureCount = 2;

	return true;
}



bool TextureArrayClass::Initialize(ID3D11Device* device, const WCHAR* filename1, const WCHAR* filename2, const WCHAR* filename3)
{
	// ù��° �ؽ��ĸ� ���Ϸκ��� �о�´�
	if (FAILED(CreateDDSTextureFromFile(device, filename1, nullptr, &m_texture[0])))
	{
		return false;
	}

	// �ι�° �ؽ��ĸ� ���Ϸκ��� �о�´�.
	if (FAILED(CreateDDSTextureFromFile(device, filename2, nullptr, &m_texture[1])))
	{
		return false;
	}

	// ����° �ؽ��ĸ� ���Ϸκ��� �о�´�.
	if (FAILED(CreateDDSTextureFromFile(device, filename3, nullptr, &m_texture[2])))
	{
		return false;
	}

	textureCount = 3;

	return true;
}

void TextureArrayClass::Shutdown()
{
	for (int i = 0; i < textureCount; i++)
	{
		// �ؽ��� �� ���ҽ��� �����Ѵ�.
		if (m_texture[i])
		{
			m_texture[i]->Release();
			m_texture[i] = 0;
		}
	}

	return;
}

ID3D11ShaderResourceView* TextureArrayClass::GetTexture(int index)
{
	return m_texture[index];
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return m_texture;
}
