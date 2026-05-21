#include "Trap.h"
#include "PlayerStatus.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include <d3d11.h>

BearTrap::BearTrap()
{
    m_position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    m_color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_isTriggered = false;
}

BearTrap::~BearTrap() {}

bool BearTrap::Init(DirectX::XMFLOAT3 pos)
{
    m_position = pos;
    m_isTriggered = false;

    m_color = DirectX::XMFLOAT4(0.5f, 0.25f, 0.1f, 1.0f);
    m_scale = DirectX::XMFLOAT3(0.6f, 0.1f, 0.6f);
    m_position.y = 0.05f;
    m_collider = AABB::Make(m_position, DirectX::XMFLOAT3(0.3f, 0.1f, 0.3f));

    return true;
}

void BearTrap::Update(float deltaTime, DirectX::XMFLOAT3& playerPos, PlayerStatus& playerStatus)
{
    // 罠の当たり判定の位置を同期
    m_collider.Move(m_position);

    if (!m_isTriggered)
    {
        AABB playerCollider = AABB::Make(playerPos, DirectX::XMFLOAT3(0.4f, 1.0f, 0.4f));

        // 罠とプレイヤーが重なったか判定
        if (m_collider.IsOverlap(playerCollider))
        {
            m_isTriggered = true;

            playerStatus.ApplyStun();
        }
    }
}

void BearTrap::Draw(ID3D11DeviceContext* context, ShaderManager* pShaderManager, Mesh* pMesh)
{
    if (context == nullptr || pShaderManager == nullptr || pMesh == nullptr) return;

    //発動した後はダークグレーに変色
    if (m_isTriggered) { m_color = DirectX::XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f); }
    else { m_color = DirectX::XMFLOAT4(0.5f, 0.25f, 0.1f, 1.0f); }

    //トラップのワールド行列を計算
    DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
    DirectX::XMMATRIX transMatrix = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

    //シェーダーの定数バッファ
    CBPerObject trapObj;
    trapObj.worldMatrix = DirectX::XMMatrixTranspose(scaleMatrix * transMatrix);
    pShaderManager->UpdatePerObject(context, trapObj);

    CBPerMaterial trapMat = { m_color, 0.9f, 0.0f, 0.0f, 1.0f };
    pShaderManager->UpdatePerMaterial(context, trapMat);

    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(1, 1, &nullSRV);

    pMesh->Draw(context);
}