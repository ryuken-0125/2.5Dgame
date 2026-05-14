#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "Texture.h"
#include "ShaderManager.h"
#include "Mesh.h"

class Effect
{
public:
    Effect();
    ~Effect();

    bool Initialize(ID3D11Device* device, const char* filePath);
    void Release();
    void Update(float deltaTime);
    void Draw(ID3D11DeviceContext* context, ShaderManager* shaderManager, Mesh* quadMesh);

    void Play(const DirectX::XMFLOAT3& position);

private:
    DirectX::XMFLOAT3 m_position;
    bool              m_isActive;
    int               m_currentFrame;
    float             m_frameTimer;

    std::unique_ptr<Texture> m_texture;
};