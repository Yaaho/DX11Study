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

bool TextureArrayClass::Initialize(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2, WCHAR* filename3)
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

	return true;
}

void TextureArrayClass::Shutdown()
{
	// �ؽ��� �� ���ҽ��� �����Ѵ�.
	if (m_texture[0])
	{
		m_texture[0]->Release();
		m_texture[0] = 0;
	}

	if (m_texture[1])
	{
		m_texture[1]->Release();
		m_texture[1] = 0;
	}

	return;
}

ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return m_texture;
}
