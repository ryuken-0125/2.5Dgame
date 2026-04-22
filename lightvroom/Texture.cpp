#include "Texture.h"
#include <vector>

// stb_imageをここで有効化するおまじない
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() {}
Texture::~Texture() {}

bool Texture::Load(ID3D11Device* device, const std::string& filePath)
{
    int width, height, channels;
    // DirectXの座標系に合わせて画像を上下反転させない（false）
    stbi_set_flip_vertically_on_load(false);

    // 画像をメモリに読み込む (必ず4チャンネル=RGBAで読み込む)
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 4);
    if (!data) return false; // 読み込み失敗（ファイルが無い等）

    // GPUに送るためのテクスチャ設定
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;
    initData.SysMemPitch = width * 4;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
    HRESULT hr = device->CreateTexture2D(&desc, &initData, &tex);

    // GPUに転送し終わったら、メインメモリの画像データは捨てる
    stbi_image_free(data);
    if (FAILED(hr)) return false;

    // シェーダー用のビューを作成
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(tex.Get(), &srvDesc, &m_srv);
    return SUCCEEDED(hr);
}