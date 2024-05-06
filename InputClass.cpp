#include "Stdafx.h"
#include "InputClass.h"


InputClass::InputClass()
{

}


InputClass::InputClass(const InputClass& other)
{

}


InputClass::~InputClass()
{

}

void InputClass::Initialize()
{
	// Ű �迭�� �ʱ�ȭ�Ѵ�
	for (int i = 0; i < 256; ++i)
	{
		m_Keys[i] = false;
	}
}

void InputClass::KeyDown(unsigned int input)
{
	// Ű�� ���ȴٸ� �ش� Ű���� true �� �����Ѵ�
	m_Keys[input] = true;
}

void InputClass::KeyUp(unsigned int input)
{
	// Ű�� �����Ǿ��ٸ� �ش� Ű���� false �� �����Ѵ�
	m_Keys[input] = false;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	// ���� Ű���� ���������� �ƴ��� ���¸� ��ȯ�Ѵ�
	return m_Keys[key];
}

