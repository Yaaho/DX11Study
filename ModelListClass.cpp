#include "Stdafx.h"
#include <time.h>
#include "ModelListClass.h"


ModelListClass::ModelListClass()
{
}


ModelListClass::ModelListClass(const ModelListClass& other)
{
}

ModelListClass::~ModelListClass()
{
}

bool ModelListClass::Initialize(int numModels)
{
	float red = 0.0f;
	float green = 0.0f;
	float blue = 0.0f;

	// �� ���� �����Ѵ�.
	m_modelCount = numModels;

	// �� ������ ����Ʈ �迭�� �����.
	m_ModelInfoList = new ModelInfoType[m_modelCount];
	if (!m_ModelInfoList)
	{
		return false;
	}

	// ���� �ð��� �õ�� ������ �����Ѵ�.
	srand((unsigned int)time(NULL));

	// ��� ���� ���� �������� �� ����� ��ġ�� �����Ѵ�.
	for (int i = 0; i < m_modelCount; i++)
	{
		// �𵨿� ������ ������ �����Ѵ�.
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;
		m_ModelInfoList[i].color = XMFLOAT4(red, green, blue, 1.0f);

		// ���� ������ ��ġ�� �����Ѵ�.
		m_ModelInfoList[i].posisionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f;
	}

	return true;
}

void ModelListClass::Shutdown()
{
	// �� ���� ����� �����Ѵ�.
	if (m_ModelInfoList)
	{
		delete[] m_ModelInfoList;
		m_ModelInfoList = 0;
	}
}

int ModelListClass::GetModelCount()
{
	return m_modelCount;
}

void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, XMFLOAT4& color)
{
	positionX = m_ModelInfoList[index].posisionX;
	positionY = m_ModelInfoList[index].positionY;
	positionZ = m_ModelInfoList[index].positionZ;

	color = m_ModelInfoList[index].color;
}