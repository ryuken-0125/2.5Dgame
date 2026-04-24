#include "ShaderManager.h"
#include <iostream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

ShaderManager::ShaderManager() {}
ShaderManager::~ShaderManager() {}

bool ShaderManager::Initialize(ID3D11Device* device, const std::string& pbrFilePath, const std::string& shadowFilePath)
{
    auto ToWString = [](const std::string& str) {
        if (str.empty()) return std::wstring();
        int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    };

    std::wstring pbrPathW = ToWString(pbrFilePath);
    std::wstring shadowPathW = ToWString(shadowFilePath);

    HRESULT hr;
    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;

    // 1. PBR Vertex Shader
    hr = D3DCompileFromFile(pbrPathW.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", compileFlags, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) { OutputErrorMessage(errorBlob.Get()); return false; }
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader);

    // 2. PBR Pixel Shader
    hr = D3DCompileFromFile(pbrPathW.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", compileFlags, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) { OutputErrorMessage(errorBlob.Get()); return false; }
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader);

    // 3. Input Layout
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    device->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);

    // 4. Shadow Vertex Shader
    vsBlob.Reset(); errorBlob.Reset();
    hr = D3DCompileFromFile(shadowPathW.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) { OutputErrorMessage(errorBlob.Get()); return false; }
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_shadowVertexShader);

    // 5. Shadow Pixel Shader
    psBlob.Reset(); errorBlob.Reset();
    hr = D3DCompileFromFile(shadowPathW.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) { OutputErrorMessage(errorBlob.Get()); return false; }
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_shadowPixelShader);

    // Sampler for textures
    D3D11_SAMPLER_DESC sampLinearDesc = {};
    sampLinearDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampLinearDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampLinearDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampLinearDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    device->CreateSamplerState(&sampLinearDesc, &m_samplerLinear);

    // Sampler for shadow map
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    device->CreateSamplerState(&sampDesc, &m_samplerClamp);

    if (!m_cbPerFrame.Initialize(device)) return false;
    if (!m_cbPerObject.Initialize(device)) return false;
    if (!m_cbPerMaterial.Initialize(device)) return false;

    return true;
}

void ShaderManager::BindShadowPass(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureSRV)
{
    context->IASetInputLayout(m_inputLayout.Get());
    context->VSSetShader(m_shadowVertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_shadowPixelShader.Get(), nullptr, 0);

    if (textureSRV) {
        context->PSSetShaderResources(0, 1, &textureSRV);
        context->PSSetSamplers(0, 1, m_samplerLinear.GetAddressOf());
    }
}

void ShaderManager::BindMainPass(ID3D11DeviceContext* context, ID3D11ShaderResourceView* shadowSRV)
{
    context->IASetInputLayout(m_inputLayout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

    context->PSSetShaderResources(0, 1, &shadowSRV);
    context->PSSetSamplers(0, 1, m_samplerLinear.GetAddressOf());
    context->PSSetSamplers(1, 1, m_samplerClamp.GetAddressOf());
}

void ShaderManager::OutputErrorMessage(ID3DBlob* errorBlob)
{
    if (errorBlob) {
        OutputDebugStringA("=== Shader Compile Error ===\n");
        OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        OutputDebugStringA("\n============================\n");
    }
    else {
        OutputDebugStringA("Shader File not found.\n");
    }
}

void ShaderManager::UpdatePerFrame(ID3D11DeviceContext* context, const CBPerFrame& data) {
    m_cbPerFrame.Data = data; m_cbPerFrame.ApplyChanges(context);
    context->VSSetConstantBuffers(0, 1, m_cbPerFrame.Buffer.GetAddressOf());
    context->PSSetConstantBuffers(0, 1, m_cbPerFrame.Buffer.GetAddressOf());
}

void ShaderManager::UpdatePerObject(ID3D11DeviceContext* context, const CBPerObject& data) {
    m_cbPerObject.Data = data; m_cbPerObject.ApplyChanges(context);
    context->VSSetConstantBuffers(1, 1, m_cbPerObject.Buffer.GetAddressOf());
    context->PSSetConstantBuffers(1, 1, m_cbPerObject.Buffer.GetAddressOf());
}

void ShaderManager::UpdatePerMaterial(ID3D11DeviceContext* context, const CBPerMaterial& data) {
    m_cbPerMaterial.Data = data; m_cbPerMaterial.ApplyChanges(context);
    context->PSSetConstantBuffers(2, 1, m_cbPerMaterial.Buffer.GetAddressOf());
}