#pragma once
#include <DirectXMath.h>
#include "ShaderManager.h"

class LightManager
{
public:
    LightManager();

    void Update(float deltaTime);

    void UpdateShadowCamera(const DirectX::XMFLOAT3& playerPos);

    CBPerFrame GetFrameData(const DirectX::XMMATRIX& viewProj, const DirectX::XMFLOAT3& cameraPos) const;

private:
    DirectX::XMFLOAT3 m_eyePos;
    DirectX::XMFLOAT3 m_lightTargetPos;
    DirectX::XMFLOAT3 m_lightMoveGoal;
    float m_lightMoveTimer;

    DirectX::XMFLOAT4 m_skyColor;

    DirectX::XMMATRIX m_lightViewProjection;
};