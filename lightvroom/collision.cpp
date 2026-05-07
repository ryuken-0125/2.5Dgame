/*------------------------------
*	当たり判定処理[collision.cpp]
*
* 制作者：山室飛龍		日付：2025/11/18
------------------------------*/
#include "collision.h"
#include <algorithm>
#include <DirectXMath.h>
using namespace DirectX;

/*====================================================================
 * AABB同士が重なっているかを判定
 * 軸ごと（x, y, z）に交差しているかをチェック
 *====================================================================*/
bool AABB::IsOverlap(const AABB& aabb)const
{
    //重なっていたらtrue
    //右の橋と左の橋が重なっていたら、ダメになる
    //ここに = をつけると、引き戻しに問題が出る
    return GetMin().x < aabb.GetMax().x
        && GetMax().x > aabb.GetMin().x
        && GetMin().y < aabb.GetMax().y
        && GetMax().y > aabb.GetMin().y
        && GetMin().z < aabb.GetMax().z
        && GetMax().z > aabb.GetMin().z;
}

/*====================================================================
 * AABB同士の衝突判定
 * ・衝突深度（押し込まれた距離）を各軸で計算
 * ・最も浅い軸方向を衝突法線として返す
 *====================================================================*/
Hit AABB::IsHit(const AABB& aabb) const
{
    // 重なっていなければ衝突情報なし（デフォルトHitを返す）
    if (!IsOverlap(aabb)) return {};

    // --- x軸方向の衝突深度 ---
    float depthX = std::min(GetMax().x, aabb.GetMax().x)
        - std::max(GetMin().x, aabb.GetMin().x);

    // --- y軸方向の衝突深度 ---
    float depthY = std::min(GetMax().y, aabb.GetMax().y)
        - std::max(GetMin().y, aabb.GetMin().y);

    // --- z軸方向の衝突深度 ---
    float depthZ = std::min(GetMax().z, aabb.GetMax().z)
        - std::max(GetMin().z, aabb.GetMin().z);

    // 最も浅い軸を求める（押し返す軸）
    int shallowAxis = 0; // x==>0, y==>1, z==>2

    if (depthX < depthY)
    {
        if (depthX < depthZ)
        {
            shallowAxis = 0;   // x方向が最も浅い
        }
        else
        {
            shallowAxis = 2;   // z方向が最も浅い
        }
    }
    else
    {
        if (depthY < depthZ)
        {
            shallowAxis = 1;   // y方向が最も浅い
        }
        else
        {
            shallowAxis = 2;   // z方向が最も浅い
        }
    }
    // --- 法線ベクトルを決定 ---
    XMFLOAT3 normal{};

    switch (shallowAxis)
    {
    case 0: // X軸方向の衝突
        // 相手の中心が自分より左 = 左から押されている → 法線は -X
        normal = (aabb.GetCenter().x - GetCenter().x < 0.0f) ?
            XMFLOAT3{ -1.0f, 0.0f, 0.0f } :
            XMFLOAT3{ 1.0f, 0.0f, 0.0f };
        break;

    case 1: // Y軸方向の衝突（上下）
        normal = (aabb.GetCenter().y - GetCenter().y < 0.0f) ?
            XMFLOAT3{ 0.0f, -1.0f, 0.0f } :
            XMFLOAT3{ 0.0f,  1.0f, 0.0f };
        break;

    case 2: // Z軸方向の衝突（前後）
        normal = (aabb.GetCenter().z - GetCenter().z < 0.0f) ?
            XMFLOAT3{ 0.0f, 0.0f, -1.0f } :
            XMFLOAT3{ 0.0f, 0.0f,  1.0f };
        break;
    }

    // --- 衝突結果を返す ---
    return { true, normal };
}
bool AABB::IsOverlap(const Point& point) const
{
    return GetMin().x < point.GetPosition().x
        && GetMax().x > point.GetPosition().x
        && GetMin().y < point.GetPosition().y
        && GetMax().y > point.GetPosition().y
        && GetMin().z < point.GetPosition().z
        && GetMax().z > point.GetPosition().z;
}
void AABB::Move(const XMFLOAT3& position)
{
    m_center = position;
    m_min = { m_center.x - m_half.x, m_center.y - m_half.y, m_center.z - m_half.z };
    m_max = { m_center.x + m_half.x, m_center.y + m_half.y, m_center.z + m_half.z };
}

void AABB::Move(const XMVECTOR& position)
{
    XMStoreFloat3(&m_center, position);
    XMStoreFloat3(&m_min, position - XMLoadFloat3(&m_half));
    XMStoreFloat3(&m_max, position + XMLoadFloat3(&m_half));
}

bool Point::IsOverlap(const AABB& aabb) const
{
    return aabb.IsOverlap(*this);
}
