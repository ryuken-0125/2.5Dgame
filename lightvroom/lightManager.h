#pragma once
#include <DirectXMath.h>
#include <d3d11.h>

// 必要に応じて既存の定数バッファ定義（CBPerFrameなど）をここに含めるか、共通ヘッダーからインクルードしてください
struct CBPerFrame;

class LightManager
{
public:
    LightManager();
    void Initialize();
    void Update(float deltaTime);

    // 影用行列の取得
    DirectX::XMMATRIX GetLightViewProjection() const { return m_lightView * m_lightProj; }

    // 定数バッファへのデータ反映
    void FillFrameData(CBPerFrame& frameData);

private:
    DirectX::XMFLOAT3 m_eyePos;          // 目の位置（光源位置）
    DirectX::XMFLOAT3 m_lightTargetPos;  // 現在の光の目標地点
    DirectX::XMFLOAT3 m_lightMoveGoal;   // 次の移動先
    float m_lightMoveTimer;              // 移動タイマー

    DirectX::XMMATRIX m_lightView;
    DirectX::XMMATRIX m_lightProj;
};