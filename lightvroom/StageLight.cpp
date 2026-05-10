#include "StageLight.h"
#include <cstdlib>

const float LIGHT_MOVE_SPEED = 1.5f;

StageLight::StageLight()
    : m_eyePosition(0.0f, 15.0f, 15.0f)
    , m_targetPosition(0.0f, 0.0f, 0.0f)
    , m_goalPosition(0.0f, 0.0f, 0.0f)
    , m_moveTimer(0.0f)
{
}

StageLight::~StageLight() {}

void StageLight::Update(float deltaTime)
{
    m_moveTimer -= deltaTime;

    if (m_moveTimer <= 0.0f)
    {
        float randomX = (float)(rand() % 200 - 100) / 10.0f;
        float randomZ = (float)(rand() % 100 - 50) / 10.0f;
        m_goalPosition.x = randomX;
        m_goalPosition.z = randomZ;

        m_moveTimer = 2.0f + (rand() % 3);
    }

    m_targetPosition.x += (m_goalPosition.x - m_targetPosition.x) * deltaTime * LIGHT_MOVE_SPEED;
    m_targetPosition.z += (m_goalPosition.z - m_targetPosition.z) * deltaTime * LIGHT_MOVE_SPEED;
}

DirectX::XMFLOAT3 StageLight::GetSpotDirection() const
{
    using namespace DirectX;
    XMVECTOR eyeVector = XMLoadFloat3(&m_eyePosition);
    XMVECTOR targetVector = XMLoadFloat3(&m_targetPosition);

    XMVECTOR directionVector = XMVector3Normalize(targetVector - eyeVector);

    XMFLOAT3 resultDirection;
    XMStoreFloat3(&resultDirection, directionVector);
    return resultDirection;
}