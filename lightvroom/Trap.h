#pragma once
#include <directxmath.h>
#include "collision.h"

class PlayerStatus;
class ShaderManager;
class Mesh;
struct ID3D11DeviceContext;

// BearTrap（トラばさみ）クラス
class BearTrap {
private:
    DirectX::XMFLOAT3 m_position;    // 罠の位置
    DirectX::XMFLOAT3 m_scale;       // 罠の大きさ
    DirectX::XMFLOAT4 m_color;       // 罠の色
    AABB              m_collider;    // 罠の当たり判定
    bool              m_isTriggered; // すでに発動したかどうかのフラグ

public:
    BearTrap();
    ~BearTrap();

    bool Init(DirectX::XMFLOAT3 pos);
    void Update(float deltaTime, DirectX::XMFLOAT3& playerPos, PlayerStatus& playerStatus);
    void Draw(ID3D11DeviceContext* context, ShaderManager* pShaderManager, Mesh* pMesh);
    bool IsTriggered() const { return m_isTriggered; }
};