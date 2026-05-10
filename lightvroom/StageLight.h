#pragma once
#include <DirectXMath.h>

/**
 * @class StageLight
 * @brief ステージ上を不気味に徘徊する上位存在の「視線（スポットライト）」を管理するクラス
 */
class StageLight
{
public:
    StageLight();
    ~StageLight();

    /**
     * @brief 毎フレームの移動処理
     * @param deltaTime 1フレームの経過時間
     */
    void Update(float deltaTime);

    /**
     * @brief 光源（目）の固定位置を取得する
     */
    DirectX::XMFLOAT3 GetEyePosition() const { return m_eyePosition; }

    /**
     * @brief 現在光が当たっている地面の位置を取得する
     */
    DirectX::XMFLOAT3 GetTargetPosition() const { return m_targetPosition; }

    /**
     * @brief 光の照射方向ベクトルを取得する
     */
    DirectX::XMFLOAT3 GetSpotDirection() const;

private:
    DirectX::XMFLOAT3 m_eyePosition;    ///< 光源（目）の固定位置
    DirectX::XMFLOAT3 m_targetPosition; ///< 現在光が当たっている地面の位置
    DirectX::XMFLOAT3 m_goalPosition;   ///< 次の移動先（ランダムで決まる）
    float m_moveTimer;                  ///< 次の移動先を決めるまでのタイマー
};