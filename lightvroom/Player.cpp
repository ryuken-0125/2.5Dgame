#include "Player.h"
#include <windows.h>
#include <cmath>

// ==========================================
// バランス調整用定数（命名規則：大文字＋アンダーバー）
// ==========================================
const float MAX_SANITY = 100.0f;
const float PLAYER_MOVE_SPEED = 5.0f;
const float SAFE_RADIUS = 10.0f;
const float SANITY_DAMAGE_PER_SECOND = 10.0f;
const float SANITY_HEAL_PER_SECOND = 5.0f;

Player::Player()
    : m_position(0.0f, 0.0f, 0.0f)
    , m_sanity(MAX_SANITY)
    , m_moveSpeed(PLAYER_MOVE_SPEED)
{
}

Player::~Player() {}

bool Player::Initialize(ID3D11Device* device)
{
    m_quadMesh = std::make_unique<Mesh>();
    if (!m_quadMesh->CreateQuad(device)) return false;

    m_texture = std::make_unique<Texture>();
    if (!m_texture->Load(device, "../asset/texture/player.png")) return false;

    return true;
}

void Player::Update(float deltaTime, const DirectX::XMFLOAT3& safeLightPosition)
{
    // ==========================================
    // 1. キーボード入力による移動 (WASD)
    // ==========================================
    float moveDistance = m_moveSpeed * deltaTime;

    if (GetAsyncKeyState('W') & 0x8000) m_position.z += moveDistance;
    if (GetAsyncKeyState('S') & 0x8000) m_position.z -= moveDistance;
    if (GetAsyncKeyState('D') & 0x8000) m_position.x += moveDistance;
    if (GetAsyncKeyState('A') & 0x8000) m_position.x -= moveDistance;

    // ==========================================
    // 2. 正気度 (Sanity) の計算
    // ==========================================
    float distanceX = m_position.x - safeLightPosition.x;
    float distanceZ = m_position.z - safeLightPosition.z;
    float distanceToLight = std::sqrt(distanceX * distanceX + distanceZ * distanceZ);

    if (distanceToLight > SAFE_RADIUS)
    {
        // 暗闇にいるとダメージ
        m_sanity -= SANITY_DAMAGE_PER_SECOND * deltaTime;
        if (m_sanity < 0.0f) m_sanity = 0.0f;
    }
    else
    {
        // 光の中にいると回復
        m_sanity += SANITY_HEAL_PER_SECOND * deltaTime;
        if (m_sanity > MAX_SANITY) m_sanity = MAX_SANITY;
    }
}

void Player::Draw(ID3D11DeviceContext* context, ShaderManager* shaderManager, bool isShadowPass)
{
    using namespace DirectX;
    CBPerObject perObjectData;

    XMMATRIX scaleMatrix = XMMatrixScaling(1.5f, 1.5f, 1.5f);
    XMMATRIX rotationMatrix = XMMatrixRotationX(XMConvertToRadians(30.0f));
    XMMATRIX translationMatrix = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

    perObjectData.worldMatrix = XMMatrixTranspose(scaleMatrix * rotationMatrix * translationMatrix);
    shaderManager->UpdatePerObject(context, perObjectData);

    CBPerMaterial materialData = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.9f, 0.0f, 0.0f, 1.0f };
    shaderManager->UpdatePerMaterial(context, materialData);

    ID3D11ShaderResourceView* shaderResourceView = m_texture->GetSRV();
    if (!isShadowPass)
    {
        context->PSSetShaderResources(1, 1, &shaderResourceView);
    }

    m_quadMesh->Draw(context);

    ID3D11ShaderResourceView* nullShaderResourceView = nullptr;
    if (!isShadowPass)
    {
        context->PSSetShaderResources(1, 1, &nullShaderResourceView);
    }
}