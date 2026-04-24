#include "LightManager.h"
#include <algorithm>
#include <cstdlib>

using namespace DirectX;

LightManager::LightManager()
    : m_eyePos(0.0f, 15.0f, 15.0f), m_lightTargetPos(0.0f, 0.0f, 0.0f),
    m_lightMoveGoal(0.0f, 0.0f, 0.0f), m_lightMoveTimer(0.0f)
{
}

void LightManager::Initialize()
{
    m_lightMoveGoal = m_lightTargetPos;
}

void LightManager::Update(float deltaTime)
{
    // 1. ライトの目標地点をランダムに更新
    m_lightMoveTimer -= deltaTime;
    if (m_lightMoveTimer <= 0.0f) {
        m_lightMoveGoal.x = (float)(rand() % 200 - 100) / 10.0f;
        m_lightMoveGoal.z = (float)(rand() % 100 - 50) / 10.0f;
        m_lightMoveTimer = 2.0f + (rand() % 3);
    }

    // ゆっくり移動
    m_lightTargetPos.x += (m_lightMoveGoal.x - m_lightTargetPos.x) * deltaTime * 1.5f;
    m_lightTargetPos.z += (m_lightMoveGoal.z - m_lightTargetPos.z) * deltaTime * 1.5f;

    // 2. ライト用行列の計算
    XMVECTOR eyeVec = XMLoadFloat3(&m_eyePos);
    XMVECTOR targetVec = XMLoadFloat3(&m_lightTargetPos);
    m_lightView = XMMatrixLookAtLH(eyeVec, targetVec, XMVectorSet(0, 1, 0, 0));
    m_lightProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 1.0f, 0.1f, 100.0f);
}

void LightManager::FillFrameData(CBPerFrame& frameData)
{
    XMVECTOR eyeVec = XMLoadFloat3(&m_eyePos);
    XMVECTOR targetVec = XMLoadFloat3(&m_lightTargetPos);
    XMVECTOR spotDirVec = XMVector3Normalize(targetVec - eyeVec);

    frameData.spotPos = m_eyePos;
    frameData.spotRange = 40.0f;
    XMStoreFloat3(&frameData.spotDir, spotDirVec);
    frameData.spotCosInner = cosf(XMConvertToRadians(10.0f));
    frameData.spotCosOuter = cosf(XMConvertToRadians(20.0f));
    frameData.spotColor = XMFLOAT3(8.0f, 8.0f, 10.0f);

    // 行列をセット（転置済み）
    frameData.lightViewProjection = XMMatrixTranspose(m_lightView * m_lightProj);
}