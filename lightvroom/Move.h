#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include "PlayerStatus.h"
#include "Effect.h"

class PlayerStatus;  // 前方宣言

class Move
{
public:
    Move();
    ~Move();

    // WASDでプレイヤーを動かす（スタン・低速をStatusから参照）
    void ControlPlayer(DirectX::XMFLOAT3& playerPos, float deltaTime,
        const PlayerStatus& status);

    // 各キーが「押した瞬間」だけ true を返す
    bool CheckFovToggle();
    bool CheckStunKey();
    bool CheckSlowKey();

	// エフェクト再生トリガーの取得等  
    bool IsEffectTriggered() const { return m_isEffectTriggered; }
    DirectX::XMFLOAT3 GetFacingDirection() const { return m_facingDirection; }
    bool IsActionTriggered() const { return m_isActionTriggered; }

private:
	// --- エフェクト再生トリガーの管理 ---
    bool m_isEffectTriggered;
    bool m_previousEffectKey;
    DirectX::XMFLOAT3 m_facingDirection = { 0.0f, 0.0f, 1.0f };
    bool m_isActionTriggered = false;
    bool m_previousActionKey = false;

    // --- 移動定数 ---
    const float BASE_MOVE_SPEED = 5.0f;
    const float DASH_SPEED      = 2.0f;  //通常時の何倍速くなるか

    // --- 追加：ジャンプ・重力定数 ---
    const float JUMP_FORCE = 8.0f;      // 跳ねる力
    const float GRAVITY    = -20.0f;    // 重力の強さ

    float m_velocityY  = 0.0f; // Y軸方向の速度
    bool  m_isGrounded = false; // 接地しているか

    // --- キーバインド ---
    const int KEY_FOV_TOGGLE = 'E';
    const int KEY_STUN = 'Q';  // スタン発動キー
    const int KEY_SLOW = 'R';  // 低速発動キー
    const int KEY_FORWARD = 'W';
    const int KEY_BACK = 'S';
    const int KEY_RIGHT = 'D';
    const int KEY_LEFT = 'A';
    const int KEY_DASH = VK_LSHIFT; //ダッシュ
    const int KEY_JUMP = VK_LCONTROL;  // スペースキーでジャンプ
};
