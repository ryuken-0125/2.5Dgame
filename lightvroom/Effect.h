#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "Mesh.h"
#include "Texture.h"
#include "ShaderManager.h"

class Effect
{
public:
    Effect();
    ~Effect();

    bool Initialize(ID3D11Device* device, const char* filePath);
    void Update(float deltaTime);
    void Draw(ID3D11DeviceContext* context, ShaderManager* shaderManager);
    void Release();

    void Play(const DirectX::XMFLOAT3& position);

private:
    DirectX::XMFLOAT3 m_position;
    bool m_isActive;
    int m_currentFrame;
    float m_frameTimer;

    std::unique_ptr<Mesh> m_quadMesh;
    std::unique_ptr<Texture> m_texture;
};