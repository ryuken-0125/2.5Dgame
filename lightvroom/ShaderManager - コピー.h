#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <string>
#include <DirectXMath.h>
#include "ConstantBuffer.h"

struct CBPerFrame
{
    DirectX::XMMATRIX viewProjection;
    DirectX::XMMATRIX lightViewProjection;
    DirectX::XMFLOAT3 cameraPos;    float pad1;

    DirectX::XMFLOAT3 sunDir;       float pad2;
    DirectX::XMFLOAT3 sunColor;     float pad3;

    DirectX::XMFLOAT3 moonDir;      float pad4;
    DirectX::XMFLOAT3 moonColor;    float pad5;

    DirectX::XMFLOAT3 lightDir;     float pad6;
    DirectX::XMFLOAT3 lightColor;   float pad7;

    DirectX::XMFLOAT3 spotPos;      float spotRange;
    DirectX::XMFLOAT3 spotDir;      float spotCosInner;
    DirectX::XMFLOAT3 spotColor;    float spotCosOuter;

    DirectX::XMFLOAT4 skyColor;  
};

struct CBPerObject {
    DirectX::XMMATRIX worldMatrix;
};

struct CBPerMaterial {
    DirectX::XMFLOAT4 albedo;
    float roughness;
    float metallic;
    float emissive;
    float pad;
    float useTexture;
};

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    bool Initialize(ID3D11Device* device, const std::string& pbrFilePath, const std::string& shadowFilePath);

    void BindShadowPass(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureSRV = nullptr);
    void BindMainPass(ID3D11DeviceContext* context, ID3D11ShaderResourceView* shadowSRV);

    void UpdatePerFrame(ID3D11DeviceContext* context, const CBPerFrame& data);
    void UpdatePerObject(ID3D11DeviceContext* context, const CBPerObject& data);
    void UpdatePerMaterial(ID3D11DeviceContext* context, const CBPerMaterial& data);

private:
    void OutputErrorMessage(ID3DBlob* errorBlob);

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shadowVertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerClamp;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_shadowPixelShader;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerLinear;

    ConstantBuffer<CBPerFrame> m_cbPerFrame;
    ConstantBuffer<CBPerObject> m_cbPerObject;
    ConstantBuffer<CBPerMaterial> m_cbPerMaterial;
};