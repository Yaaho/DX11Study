#include "Stdafx.h"
#include "FontClass.h"
#include "FontShaderClass.h"
#include "TextClass.h"


TextClass::TextClass()
{
}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth,
    int screenHeight, XMMATRIX baseViewMatrix)
{
    // ȭ�� �ʺ�� ���̸� �����Ѵ�.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // �⺻ �� ��Ʈ������ �����Ѵ�.
    m_baseViewMatrix = baseViewMatrix;

    // ��Ʈ ��ü�� �����Ѵ�.
    m_Font = new FontClass;
    if (!m_Font)
    {
        return false;
    }

    // ��Ʈ ��ü�� �ʱ�ȭ �Ѵ�.
    if (!m_Font->Initialize(device, "data/fontdata.txt", L"data/font.dds"))
    {
        MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
        return false;
    }

    // ��Ʈ ���̴� ��ü�� �����Ѵ�.
    m_FontShader = new FontShaderClass;
    if (!m_FontShader)
    {
        return false;
    }

    // ��Ʈ ���̴� ��ü�� �ʱ�ȭ �Ѵ�.
    if (!m_FontShader->Initialize(device, hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
        return false;
    }

    // ù��° ������ �ʱ�ȭ�Ѵ�.
    if (!InitializeSentence(&m_sentence1, 16, device))
    {
        return false;
    }

    // ���� ���� ���۸� �� ���ڿ� ������ ������Ʈ�Ѵ�.
    if (!UpdateSentence(m_sentence1, "Render Count: ", 20, 20, 1.0f, 1.0f, 1.0f, deviceContext))
    {
        return false;
    }

    return true;
}


void TextClass::Shutdown()
{
    // ù ��° ������ ��ȯ�Ѵ�.
    ReleaseSentence(&m_sentence1);

    // ��Ʈ ���̴� ��ü�� ��ȯ�Ѵ�.
    if (m_FontShader)
    {
        m_FontShader->Shutdown();
        delete m_FontShader;
        m_FontShader = 0;
    }

    // ��Ʈ ��ü�� ��ȯ�Ѵ�.
    if (m_Font)
    {
        m_Font->Shutdown();
        delete m_Font;
        m_Font = 0;
    }

    return;
}


bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
    // ù ��° ������ �׸���.
    return RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
}


bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
    // ���ο� ���� ��ü�� �����.
    *sentence = new SentenceType;
    if (!*sentence)
    {
        return false;
    }

    // ���� ���۸� null �� �ʱ�ȭ�Ѵ�.
    (*sentence)->vertexBuffer = 0;
    (*sentence)->indexBuffer = 0;

    // ������ �ִ� ���̸� �����Ѵ�.
    (*sentence)->maxLength = maxLength;

    // ���� �迭�� ���� ���� �����Ѵ�.
    (*sentence)->vertexCount = 6 * maxLength;

    // �ε��� �迭�� �ε��� ���� �����Ѵ�.
    (*sentence)->indexCount = (*sentence)->vertexCount;

    // ���� �迭�� �����.
    VertexType* vertices = new VertexType[(*sentence)->vertexCount];
    if (!vertices)
    {
        return false;
    }

    // �ε��� �迭�� �����.
    unsigned long* indices = new unsigned long[(*sentence)->indexCount];
    if (!indices)
    {
        return false;
    }

    // ���� �迭�� 0���� �ʱ�ȭ�Ѵ�.
    memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

    // �ε��� �迭�� 0���� �ʱ�ȭ�Ѵ�.
    for (int i = 0; i < (*sentence)->indexCount; i++)
    {
        indices[i] = i;
    }

    // ���ؽ� ���۸� ���� ���۷� �����Ѵ�.
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // SUBRESOURCE �� ���� �������� �����͸� �����Ѵ�.
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // ���ؽ� ���۸� �����.
    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer)))
    {
        return false;
    }

    // �ε��� ������ Description �� �����Ѵ�.
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // SUBRESOURCE �� �ε��� �������� �����͸� �����Ѵ�.
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // �ε��� ���۸� �����.
    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer)))
    {
        return false;
    }

    // �� �̻� �ʿ����� ���� ���� �迭�� �����Ѵ�.
    delete[] vertices;
    vertices = 0;

    // �� �̻� �ʿ����� ���� �ε��� �迭�� �����Ѵ�.
    delete[] indices;
    indices = 0;

    return true;
}


bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green,
    float blue, ID3D11DeviceContext* deviceContext)
{
    // ������ ���� �����Ѵ�.
    sentence->red = red;
    sentence->green = green;
    sentence->blue = blue;

    // ������ ���� �����÷ο츦 Ȯ���Ѵ�.
    if ((int)strlen(text) > sentence->maxLength)
    {
        return false;
    }

    // ���� �迭�� �����.
    VertexType* vertices = new VertexType[sentence->vertexCount];
    if (!vertices)
    {
        return false;
    }

    // ���� �迭�� 0���� �ʱ�ȭ�Ѵ�.
    memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

    // �׸��⸦ ������ ȭ�鿡�� X �� Y �ȼ� ��ġ�� ����Ѵ�.
    float drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
    float drawY = (float)((m_screenHeight / 2) - positionY);

    // ��Ʈ Ŭ������ ����Ͽ� ���� �ؽ�Ʈ�� ���� �׸��� ��ġ���� ���� �迭�� �����.
    m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

    // ���ؽ� ���۸� �� �� �ֵ��� ��ٴ�.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // ���� ������ �����͸� ����Ű�� �����͸� ��´�.
    VertexType* verticesPtr = (VertexType*)mappedResource.pData;
    
    // �����͸� ���� ���ۿ� �����Ѵ�.
    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

    // ���� ������ ����� �����Ѵ�.
    deviceContext->Unmap(sentence->vertexBuffer, 0);

    delete[] vertices;
    vertices = 0;

    return true;
}


void TextClass::ReleaseSentence(SentenceType** sentence)
{
    if (*sentence)
    {
        // ������ ���ؽ� ���۸� �����Ѵ�.
        if ((*sentence)->vertexBuffer)
        {
            (*sentence)->vertexBuffer->Release();
            (*sentence)->vertexBuffer = 0;
        }

        // ������ �ε��� ���۸� �����Ѵ�.
        if ((*sentence)->indexBuffer)
        {
            (*sentence)->indexBuffer->Release();
            (*sentence)->indexBuffer = 0;
        }

        // ������ �����Ѵ�.
        delete* sentence;
        *sentence = 0;
    }

    return;
}


bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix,
    XMMATRIX orthoMatrix)
{
    // ���� ���� ���� �� �������� �����Ѵ�.
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    // ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����Ѵ�.
    deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

    // ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����Ѵ�.
    deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // �� ���� ���ۿ��� ������ �Ǿ�� �ϴ� ������Ƽ�� ������ �����Ѵ�. �� ��쿡�� �ﰢ���̴�.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // �Էµ� ���� �������� �ȼ� ���� ���͸� �����.
    XMFLOAT4 pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

    // ��Ʈ ���̴��� ����Ͽ� �ؽ�Ʈ�� �������Ѵ�.
    if (!m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix,
        m_Font->GetTexture(), pixelColor))
    {
        false;
    }

    return true;
}

bool TextClass::SetRenderCount(int count, ID3D11DeviceContext* deviceContext)
{
    char tempString[32] = { 0, };
    char countString[32] = { 0, };

    // count ������ ���ڿ� �������� ��ȯ�Ѵ�.
    _itoa_s(count, tempString, 10);

    // render count ���ڿ� ����
    strcpy_s(countString, "Render Count: ");
    strcat_s(countString, tempString);

    // ���� ���۸� �� ���ڿ� ������ �����Ѵ�.
    return UpdateSentence(m_sentence1, countString, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);
}