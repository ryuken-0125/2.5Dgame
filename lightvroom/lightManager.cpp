#include "lightManager.h"
#include <stdlib.h>

using namespace DirectX;

LightManager::LightManager()
{
    m_eyePos = XMFLOAT3(0.0f, 15.0f, 15.0f);
    m_lightTargetPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_lightMoveGoal = m_lightTargetPos;
    m_lightMoveTimer = 0.0f;
    m_skyColor = XMFLOAT4(0.02f, 0.02f, 0.08f, 1.0f);
}

void LightManager::Update(float deltaTime)
{
    m_lightMoveTimer -= deltaTime;
    if (m_lightMoveTimer <= 0.0f) {
        m_lightMoveGoal.x = (float)(rand() % 200 - 100) / 10.0f;
        m_lightMoveGoal.z = (float)(rand() % 100 - 50) / 10.0f;
        m_lightMoveTimer = 2.0f + (rand() % 3);
    }

    m_lightTargetPos.x += (m_lightMoveGoal.x - m_lightTargetPos.x) * deltaTime * 1.5f;
    m_lightTargetPos.z += (m_lightMoveGoal.z - m_lightTargetPos.z) * deltaTime * 1.5f;

    m_skyColor = XMFLOAT4(0.02f, 0.02f, 0.08f, 1.0f);
}

void LightManager::UpdateShadowCamera(const DirectX::XMFLOAT3& playerPos)
{
    XMVECTOR eyeVec = XMLoadFloat3(&m_eyePos);
    XMVECTOR targetVec = XMLoadFloat3(&m_lightTargetPos);

    XMMATRIX lightView = XMMatrixLookAtLH(eyeVec, targetVec, XMVectorSet(0, 1, 0, 0));
    XMMATRIX lightProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 1.0f, 0.1f, 100.0f);

    m_lightViewProjection = XMMatrixTranspose(lightView * lightProj);
}

CBPerFrame LightManager::GetFrameData(const XMMATRIX& viewProj, const XMFLOAT3& cameraPos) const
{
    XMVECTOR eyeVec = XMLoadFloat3(&m_eyePos);
    XMVECTOR targetVec = XMLoadFloat3(&m_lightTargetPos);
    XMVECTOR spotDirVec = XMVector3Normalize(targetVec - eyeVec);

    CBPerFrame data = {};
    data.viewProjection = XMMatrixTranspose(viewProj);
    data.lightViewProjection = m_lightViewProjection;
    data.cameraPos = cameraPos;
    
    data.spotPos = m_eyePos;
    data.spotRange = 40.0f;
    XMStoreFloat3(&data.spotDir, spotDirVec);
    data.spotCosInner = cosf(XMConvertToRadians(10.0f));
    data.spotCosOuter = cosf(XMConvertToRadians(20.0f));
    data.spotColor = XMFLOAT3(8.0f, 8.0f, 10.0f);

    data.skyColor = m_skyColor;

    return data;
}