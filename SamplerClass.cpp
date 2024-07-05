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

    // 랩 텍스처 샘플러 상태 구조체를 설정합니다.
    D3D11_SAMPLER_DESC LinearSamplerDesc;
    LinearSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    LinearSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    LinearSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    LinearSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    LinearSamplerDesc.MipLODBias = 0.0f;
    LinearSamplerDesc.MaxAnisotropy = 1;
    LinearSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    LinearSamplerDesc.BorderColor[0] = 0;
    LinearSamplerDesc.BorderColor[1] = 0;
    LinearSamplerDesc.BorderColor[2] = 0;
    LinearSamplerDesc.BorderColor[3] = 0;
    LinearSamplerDesc.MinLOD = 0;
    LinearSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    //텍스처 샘플러 상태를 만듭니다.
    result = device->CreateSamplerState(&LinearSamplerDesc, &m_LinearSamplerState);
    if (FAILED(result))
    {
        return false;
    }


    // 랩 텍스처 샘플러 상태 구조체를 설정합니다.
    D3D11_SAMPLER_DESC PointSamplerDesc;
    PointSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    PointSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    PointSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    PointSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    PointSamplerDesc.MipLODBias = 0.0f;
    PointSamplerDesc.MaxAnisotropy = 1;
    PointSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    PointSamplerDesc.BorderColor[0] = 0;
    PointSamplerDesc.BorderColor[1] = 0;
    PointSamplerDesc.BorderColor[2] = 0;
    PointSamplerDesc.BorderColor[3] = 0;
    PointSamplerDesc.MinLOD = 0;
    PointSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    //텍스처 샘플러 상태를 만듭니다.
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
    // 픽셀 쉐이더에서 샘플러 상태를 설정합니다.
    deviceContext->PSSetSamplers(slot, 1, &m_LinearSamplerState);
}


void SamplerClass::UsePoint(ID3D11DeviceContext* deviceContext, int slot)
{
    // 픽셀 쉐이더에서 샘플러 상태를 설정합니다.
    deviceContext->PSSetSamplers(slot, 1, &m_PointSamplerState);
}
