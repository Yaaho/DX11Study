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
    // 화면 너비와 높이를 저장한다.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // 기본 뷰 매트릭스를 저장한다.
    m_baseViewMatrix = baseViewMatrix;

    // 폰트 객체를 생성한다.
    m_Font = new FontClass;
    if (!m_Font)
    {
        return false;
    }

    // 폰트 객체를 초기화 한다.
    if (!m_Font->Initialize(device, "data/fontdata.txt", L"data/font.dds"))
    {
        MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
        return false;
    }

    // 폰트 셰이더 객체를 생성한다.
    m_FontShader = new FontShaderClass;
    if (!m_FontShader)
    {
        return false;
    }

    // 폰트 셰이더 객체를 초기화 한다.
    if (!m_FontShader->Initialize(device, hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
        return false;
    }

    // 첫번째 문장을 초기화한다.
    if (!InitializeSentence(&m_sentence1, 32, device))
    {
        return false;
    }

    // 문장 정점 버퍼를 새 문자열 정보로 업데이트한다.
    if (!UpdateSentence(m_sentence1, "Intersection: No", 20, 20, 1.0f, 0.0f, 0.0f, deviceContext))
    {
        return false;
    }

    return true;
}


void TextClass::Shutdown()
{
    // 첫 번째 문장을 반환한다.
    ReleaseSentence(&m_sentence1);

    // 폰트 셰이더 객체를 반환한다.
    if (m_FontShader)
    {
        m_FontShader->Shutdown();
        delete m_FontShader;
        m_FontShader = 0;
    }

    // 폰트 객체를 반환한다.
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
    // 첫 번째 문장을 그린다.
    return RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
}


bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
    // 새로운 문장 객체를 만든다.
    *sentence = new SentenceType;
    if (!*sentence)
    {
        return false;
    }

    // 문장 버퍼를 null 로 초기화한다.
    (*sentence)->vertexBuffer = 0;
    (*sentence)->indexBuffer = 0;

    // 문장의 최대 길이를 설정한다.
    (*sentence)->maxLength = maxLength;

    // 정점 배열의 정점 수를 설정한다.
    (*sentence)->vertexCount = 6 * maxLength;

    // 인덱스 배열의 인덱스 수를 설정한다.
    (*sentence)->indexCount = (*sentence)->vertexCount;

    // 정점 배열을 만든다.
    VertexType* vertices = new VertexType[(*sentence)->vertexCount];
    if (!vertices)
    {
        return false;
    }

    // 인덱스 배열을 만든다.
    unsigned long* indices = new unsigned long[(*sentence)->indexCount];
    if (!indices)
    {
        return false;
    }

    // 정점 배열을 0으로 초기화한다.
    memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

    // 인덱스 배열을 0으로 초기화한다.
    for (int i = 0; i < (*sentence)->indexCount; i++)
    {
        indices[i] = i;
    }

    // 버텍스 버퍼를 동적 버퍼로 설정한다.
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // SUBRESOURCE 에 정점 데이터의 포인터를 제공한다.
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // 버텍스 버퍼를 만든다.
    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer)))
    {
        return false;
    }

    // 인덱스 버퍼의 Description 을 설정한다.
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // SUBRESOURCE 에 인덱스 데이터의 포인터를 제공한다.
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // 인덱스 버퍼를 만든다.
    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer)))
    {
        return false;
    }

    // 더 이상 필요하지 않은 정점 배열을 해제한다.
    delete[] vertices;
    vertices = 0;

    // 더 이상 필요하지 않은 인덱스 배열을 해제한다.
    delete[] indices;
    indices = 0;

    return true;
}


bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green,
    float blue, ID3D11DeviceContext* deviceContext)
{
    // 문장의 색을 저장한다.
    sentence->red = red;
    sentence->green = green;
    sentence->blue = blue;

    // 가능한 버퍼 오버플로우를 확인한다.
    if ((int)strlen(text) > sentence->maxLength)
    {
        return false;
    }

    // 정점 배열을 만든다.
    VertexType* vertices = new VertexType[sentence->vertexCount];
    if (!vertices)
    {
        return false;
    }

    // 정점 배열을 0으로 초기화한다.
    memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

    // 그리기를 시작할 화면에서 X 및 Y 픽셀 위치를 계산한다.
    float drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
    float drawY = (float)((m_screenHeight / 2) - positionY);

    // 폰트 클래스를 사용하여 문장 텍스트와 문장 그리기 위치에서 정점 배열을 만든다.
    m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

    // 버텍스 버퍼를 쓸 수 있도록 잠근다.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // 정점 버퍼의 데이터를 가리키는 포인터를 얻는다.
    VertexType* verticesPtr = (VertexType*)mappedResource.pData;
    
    // 데이터를 정점 버퍼에 복사한다.
    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

    // 정점 버퍼의 잠금을 해제한다.
    deviceContext->Unmap(sentence->vertexBuffer, 0);

    delete[] vertices;
    vertices = 0;

    return true;
}


void TextClass::ReleaseSentence(SentenceType** sentence)
{
    if (*sentence)
    {
        // 문장의 버텍스 버퍼를 해제한다.
        if ((*sentence)->vertexBuffer)
        {
            (*sentence)->vertexBuffer->Release();
            (*sentence)->vertexBuffer = 0;
        }

        // 문장의 인덱스 버퍼를 해제한다.
        if ((*sentence)->indexBuffer)
        {
            (*sentence)->indexBuffer->Release();
            (*sentence)->indexBuffer = 0;
        }

        // 문장을 해제한다.
        delete* sentence;
        *sentence = 0;
    }

    return;
}


bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, 
    XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
    // 정점 버퍼 간격 및 오프셋을 설정한다.
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    // 렌더링 할 수 있도록 입력 어셈블러에서 정적 버퍼를 활성으로 설정한다.
    deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

    // 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정한다.
    deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 이 정점 버퍼에서 렌더링 되어야 하는 프리미티브 유형을 설정한다. 이 경우에는 삼각형이다.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 입력된 문장 색상으로 픽셀 색상 벡터를 만든다.
    XMFLOAT4 pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

    // 폰트 셰이더를 사용하여 텍스트를 렌더링한다.
    if (!m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix,
        m_Font->GetTexture(), pixelColor))
    {
        false;
    }

    return true;
}

bool TextClass::SetIntersection(bool intersection, ID3D11DeviceContext* deviceContext)
{
    char intersectionString[32] = { 0, };
    bool result = false;

    if (intersection)
    {
        strcpy_s(intersectionString, "Intersection: Yes");
        result = UpdateSentence(m_sentence1, intersectionString, 20, 20, 0.0f, 1.0f, 0.0f, deviceContext);
    }
    else
    {
        strcpy_s(intersectionString, "Intersection: No");
        result = UpdateSentence(m_sentence1, intersectionString, 20, 20, 1.0f, 0.0f, 0.0f, deviceContext);
    }

    return result;
}