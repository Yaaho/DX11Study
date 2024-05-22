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
	// 첫번째 텍스쳐를 파일로부터 읽어온다
	if (FAILED(CreateDDSTextureFromFile(device, filename1, nullptr, &m_texture[0])))
	{
		return false;
	}

	// 두번째 텍스쳐를 파일로부터 읽어온다.
	if (FAILED(CreateDDSTextureFromFile(device, filename2, nullptr, &m_texture[1])))
	{
		return false;
	}

	// 세번째 텍스쳐를 파일로부터 읽어온다.
	if (FAILED(CreateDDSTextureFromFile(device, filename3, nullptr, &m_texture[2])))
	{
		return false;
	}

	return true;
}

void TextureArrayClass::Shutdown()
{
	// 텍스쳐 뷰 리소스를 해제한다.
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
