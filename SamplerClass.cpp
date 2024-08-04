#include "Stdafx.h"
#include "SamplerClass.h"

SamplerClass::SamplerClass()
{

}

SamplerClass::SamplerClass(const SamplerClass& other)
{

}

SamplerClass::~SamplerClass()
{

}


bool SamplerClass::Initialize(ID3D11Device* device)
{
    HRESULT result;

    // �� �ؽ�ó ���÷� ���� ����ü�� �����մϴ�.
    D3D11_SAMPLER_DESC LinearSamplerDesc;
    LinearSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    LinearSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    LinearSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    LinearSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    LinearSamplerDesc.MipLODBias = 0.0f;
    LinearSamplerDesc.MaxAnisotropy = 1;
    LinearSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    LinearSamplerDesc.BorderColor[0] = 1.0f;
    LinearSamplerDesc.BorderColor[1] = 1.0f;
    LinearSamplerDesc.BorderColor[2] = 1.0f;
    LinearSamplerDesc.BorderColor[3] = 1.0f;
    LinearSamplerDesc.MinLOD = 0;
    LinearSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    //�ؽ�ó ���÷� ���¸� ����ϴ�.
    result = device->CreateSamplerState(&LinearSamplerDesc, &m_LinearSamplerState);
    if (FAILED(result))
    {
        return false;
    }


    // �� �ؽ�ó ���÷� ���� ����ü�� �����մϴ�.
    D3D11_SAMPLER_DESC PointSamplerDesc;
    PointSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    PointSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    PointSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    PointSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    PointSamplerDesc.MipLODBias = 0.0f;
    PointSamplerDesc.MaxAnisotropy = 1;
    PointSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    PointSamplerDesc.BorderColor[0] = 1.0f;
    PointSamplerDesc.BorderColor[1] = 1.0f;
    PointSamplerDesc.BorderColor[2] = 1.0f;
    PointSamplerDesc.BorderColor[3] = 1.0f;
    PointSamplerDesc.MinLOD = 0;
    PointSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    //�ؽ�ó ���÷� ���¸� ����ϴ�.
    result = device->CreateSamplerState(&PointSamplerDesc, &m_PointSamplerState);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}


void SamplerClass::Shutdown()
{
    if (m_LinearSamplerState)
    {
        m_LinearSamplerState->Release();
        m_LinearSamplerState = 0;
    }

    if (m_PointSamplerState)
    {
        m_PointSamplerState->Release();
        m_PointSamplerState = 0;
    }
}



void SamplerClass::UseLinear(ID3D11DeviceContext* deviceContext, int slot)
{
    // �ȼ� ���̴����� ���÷� ���¸� �����մϴ�.
    deviceContext->PSSetSamplers(slot, 1, &m_LinearSamplerState);
}


void SamplerClass::UsePoint(ID3D11DeviceContext* deviceContext, int slot)
{
    // �ȼ� ���̴����� ���÷� ���¸� �����մϴ�.
    deviceContext->PSSetSamplers(slot, 1, &m_PointSamplerState);
}
