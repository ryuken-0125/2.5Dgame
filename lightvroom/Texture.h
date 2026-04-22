#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

class Texture
{
public:
    Texture();
    ~Texture();

    // 相対パスで画像を読み込む
    bool Load(ID3D11Device* device, const std::string& filePath);

    // シェーダーに渡すための窓口
    ID3D11ShaderResourceView* GetSRV() { return m_srv.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
};