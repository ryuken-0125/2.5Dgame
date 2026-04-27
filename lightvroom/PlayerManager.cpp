#include "PlayerManager.h"
#include "Move.h"
#include "PlayerStatus.h"
#include "Texture.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "Mesh.h"

using namespace DirectX;

PlayerManager::PlayerManager()
    : m_playerPosition(0.0f, 0.0f, 0.0f)
{
}

PlayerManager::~PlayerManager()
{
}

bool PlayerManager::Initialize(ID3D11Device* device)
{
    m_move = std::make_unique<Move>();
    m_status = std::make_unique<PlayerStatus>();
    m_status->Initialize();

    m_playerTexture = std::make_unique<Texture>();
    if (!m_playerTexture->Load(device, "asset/texture/player.png")) return false;

    return true;
}

void PlayerManager::Update(float deltaTime, Camera& camera)
{
    //移動の更新
    m_move->ControlPlayer(m_playerPosition, deltaTime);

    //ステータスの更新（ダッシュは後で追加するため、現在はfalse固定）
    m_status->Update(deltaTime, false);

    //カメラの追従設定
    const XMFLOAT3 cameraOffset(0.0f, 4.0f, -6.0f);
    camera.SetFollowTarget(m_playerPosition, cameraOffset);
}

void PlayerManager::Draw(ID3D11DeviceContext* context, ShaderManager* shaderManager, Mesh* quadMesh, bool isShadowPass)
{
    XMMATRIX scale = XMMatrixScaling(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE);
    XMMATRIX rot = XMMatrixRotationX(XMConvertToRadians(30.0f));
    XMMATRIX trans = XMMatrixTranslation(m_playerPosition.x, m_playerPosition.y, m_playerPosition.z);

    CBPerObject playerObj;
    playerObj.worldMatrix = XMMatrixTranspose(scale * rot * trans);
    shaderManager->UpdatePerObject(context, playerObj);

    CBPerMaterial playerMat = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.9f, 0.0f, 0.0f, 1.0f };
    shaderManager->UpdatePerMaterial(context, playerMat);

        ID3D11ShaderResourceView* pSRV = m_playerTexture->GetSRV();
    if (!isShadowPass) context->PSSetShaderResources(1, 1, &pSRV);

    quadMesh->Draw(context);

        ID3D11ShaderResourceView* nullSRV = nullptr;
    if (!isShadowPass) context->PSSetShaderResources(1, 1, &nullSRV);
}

ID3D11ShaderResourceView* PlayerManager::GetTextureSRV() const
{
    return m_playerTexture->GetSRV();
}
bool PlayerManager::CheckFovToggle() const
{
    return m_move->CheckFovToggle();
}
