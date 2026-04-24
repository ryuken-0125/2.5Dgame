#pragma once
#include <memory>
#include <DirectXMath.h>
#include <d3d11.h>

class Move;
class PlayerStatus;
class Texture;
class Camera;
class ShaderManager;
class Mesh;

class PlayerManager
{
public:
    PlayerManager();
    ~PlayerManager();

    bool Initialize(ID3D11Device* device);
    void Update(float deltaTime, Camera& camera);
    void Draw(ID3D11DeviceContext* context, ShaderManager* shaderManager, Mesh* quadMesh, bool isShadowPass);


    DirectX::XMFLOAT3 GetPosition() const { return m_playerPosition; }
    ID3D11ShaderResourceView* GetTextureSRV() const;
    Texture* GetTexture() const { return m_playerTexture.get(); }
    bool CheckFovToggle() const;

private:
    DirectX::XMFLOAT3 m_playerPosition;

    std::unique_ptr<Move>         m_move;
    std::unique_ptr<PlayerStatus> m_status;
    std::unique_ptr<Texture>      m_playerTexture;

    const float PLAYER_SCALE = 1.5f;
};
