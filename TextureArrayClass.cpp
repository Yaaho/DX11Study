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
	// 첫번째 텍스쳐를 파일로부터 읽어온다
	if (FAILED(CreateDDSTextureFromFile(device, filename1, nullptr, &m_texture[0])))
	{
		return false;
	}

	textureCount = 1;

	return true;
}

bool TextureArrayClass::Initialize(ID3D11Device* device, const WCHAR* filename1, const WCHAR* filename2)
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

	textureCount = 2;

	return true;
}



bool TextureArrayClass::Initialize(ID3D11Device* device, const WCHAR* filename1, const WCHAR* filename2, const WCHAR* filename3)
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

	textureCount = 3;

	return true;
}

bool TextureArrayClass::Initialize(ID3D11Device* device, const WCHAR* filename1, const WCHAR* filename2, const WCHAR* filename3, const WCHAR* filename4)
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

	// 네번째 텍스쳐를 파일로부터 읽어온다.
	if (FAILED(CreateDDSTextureFromFile(device, filename4, nullptr, &m_texture[3])))
	{
		return false;
	}

	textureCount = 4;

	return true;
}

bool TextureArrayClass::Initialize(ID3D11Device* device, const WCHAR* filename1, const WCHAR* filename2, const WCHAR* filename3, const WCHAR* filename4, const WCHAR* filename5)
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

	// 네번째 텍스쳐를 파일로부터 읽어온다.
	if (FAILED(CreateDDSTextureFromFile(device, filename4, nullptr, &m_texture[3])))
	{
		return false;
	}

	// 다섯번째 텍스쳐를 파일로부터 읽어온다.
	if (FAILED(CreateDDSTextureFromFile(device, filename5, nullptr, &m_texture[4])))
	{
		return false;
	}

	textureCount = 5;
	
	return true;
}


void TextureArrayClass::Shutdown()
{
	for (int i = 0; i < textureCount; i++)
	{
		// 텍스쳐 뷰 리소스를 해제한다.
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
