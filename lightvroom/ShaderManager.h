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
    // ライト（共通）
    DirectX::XMFLOAT3 lightDir;     float pad2;
    DirectX::XMFLOAT3 lightColor;   float pad3;
    // 太陽
    DirectX::XMFLOAT3 sunDir;       float pad4;
    DirectX::XMFLOAT3 sunColor;     float pad5;
    // 月
    DirectX::XMFLOAT3 moonDir;      float pad6;
    DirectX::XMFLOAT3 moonColor;    float pad7;

    DirectX::XMFLOAT3 spotPos;      float spotRange;    
    DirectX::XMFLOAT3 spotDir;      float spotCosInner; 
    DirectX::XMFLOAT3 spotColor;    float spotCosOuter; 

    // 空の色
    DirectX::XMFLOAT4 skyColor;
};

struct CBPerObject
{
    DirectX::XMMATRIX worldMatrix;
};

// [Fix1] useTexture を pad の前に置くと HLSL 側 float4 アライメントがズレる。
// HLSL の cbuffer は float4 境界でパックされるため、
//   albedo(float4) + roughness(float) + metallic(float) + emissive(float) + useTexture(float)
// の順にすることで 1 つの float4 に収まりパディング不要になる。
struct CBPerMaterial
{
    DirectX::XMFLOAT4 albedo;
    float roughness;
    float metallic;
    float emissive;
    float useTexture; // [Fix1] pad と useTexture を入れ替え（HLSL レイアウト合わせ）
};

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    bool Initialize(ID3D11Device* device,
        const std::wstring& pbrFilePath,
        const std::wstring& shadowFilePath);

    void BindShadowPass(ID3D11DeviceContext* context,
        ID3D11ShaderResourceView* textureSRV = nullptr);
    void BindMainPass(ID3D11DeviceContext* context,
        ID3D11ShaderResourceView* shadowSRV);

    void UpdatePerFrame(ID3D11DeviceContext* context, const CBPerFrame& data);
    void UpdatePerObject(ID3D11DeviceContext* context, const CBPerObject& data);
    void UpdatePerMaterial(ID3D11DeviceContext* context, const CBPerMaterial& data);

private:
    void OutputErrorMessage(ID3DBlob* errorBlob);

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shadowVertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_shadowPixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerClamp;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerLinear;

    ConstantBuffer<CBPerFrame>    m_cbPerFrame;
    ConstantBuffer<CBPerObject>   m_cbPerObject;
    ConstantBuffer<CBPerMaterial> m_cbPerMaterial;
};