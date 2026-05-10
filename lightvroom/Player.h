#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "Mesh.h"
#include "Texture.h"
#include "ShaderManager.h"

/**
 * @class Player
 * @brief プレイヤーキャラクターの移動、ステータス(正気度)、描画を管理するクラス
 * * @details チーム制作時の注意：
 * プレイヤーの移動速度や、正気度の増減値などのゲームバランスに関わるパラメータは、
 * Player.cpp内の定数（PLAYER_MOVE_SPEEDなど）を変更して調整してください。
 */
class Player
{
public:
    Player();
    ~Player();

    /**
     * @brief プレイヤーの初期化（モデルや画像の読み込み）
     * @param device DirectX11のデバイス
     * @return 成功した場合はtrue、失敗した場合はfalse
     */
    bool Initialize(ID3D11Device* device);

    /**
     * @brief 毎フレームの更新処理（移動と正気度の計算）
     * @param deltaTime 1フレームの経過時間
     * @param safeLightPosition 現在のスポットライトが照らしている地面の座標
     */
    void Update(float deltaTime, const DirectX::XMFLOAT3& safeLightPosition);

    /**
     * @brief プレイヤーの描画処理
     * @param context DirectX11のデバイスコンテキスト
     * @param shaderManager シェーダーマネージャーのポインタ
     * @param isShadowPass 影描画パスかどうかのフラグ
     */
    void Draw(ID3D11DeviceContext* context, ShaderManager* shaderManager, bool isShadowPass);

    /**
     * @brief プレイヤーの現在位置を取得する
     * @return プレイヤーの現在座標
     */
    DirectX::XMFLOAT3 GetPosition() const { return m_position; }

private:
    DirectX::XMFLOAT3 m_position;        ///< プレイヤーの現在位置
    float m_sanity;                      ///< 正気度 (0.0f ～ 100.0f)
    float m_moveSpeed;                   ///< 移動速度

    std::unique_ptr<Mesh> m_quadMesh;    ///< 2D板ポリゴン
    std::unique_ptr<Texture> m_texture;  ///< キャラクター画像
};