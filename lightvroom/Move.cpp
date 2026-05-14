#include "Move.h"
#include "PlayerStatus.h"
#include "Effect.h"
#include <cmath>

//Move::Move() {}

Move::Move() : m_isEffectTriggered(false), m_previousEffectKey(false) {}
Move::~Move() {}

// コンストラクタを更新

/*====================================================================
 * プレイヤー移動制御
 * ・スタン中は一切移動しない
 * ・低速中は速度に倍率を掛ける
 *====================================================================*/
void Move::ControlPlayer(DirectX::XMFLOAT3& playerPos, float deltaTime, 
    const PlayerStatus& status)
{
    // スタン中は移動処理をスキップ
    if (status.IsStunned()) return;

    // ダッシュ判定
    bool isDashing = (GetAsyncKeyState(KEY_DASH) & 0x8000) != 0;

    float speed = BASE_MOVE_SPEED;
    if (isDashing)
    {
        speed *= DASH_SPEED; // ダッシュ倍率（2倍）を適用
    }

    // 低速状態なら速度倍率を適用
    speed *= status.GetSpeedMultiplier() * deltaTime;

	// 移動入力の処理  
    bool isMoving = false;
    DirectX::XMFLOAT3 moveVector = { 0.0f, 0.0f, 0.0f };

    if (GetAsyncKeyState(KEY_FORWARD) & 0x8000) playerPos.z += speed;  // 奥へ
    if (GetAsyncKeyState(KEY_BACK) & 0x8000) playerPos.z -= speed;  // 手前へ
    if (GetAsyncKeyState(KEY_RIGHT) & 0x8000) playerPos.x += speed;  // 右へ
    if (GetAsyncKeyState(KEY_LEFT) & 0x8000) playerPos.x -= speed;  // 左へ

    // 移動キーが押されていたら、向いている方向を更新
    if (isMoving)
    {
        // ベクトルを正規化（斜め移動でも長さを1にする計算）
        float vectorLength = std::sqrt(moveVector.x * moveVector.x + moveVector.z * moveVector.z);
        if (vectorLength > 0.0f)
        {
            m_facingDirection.x = moveVector.x / vectorLength;
            m_facingDirection.y = 0.0f;
            m_facingDirection.z = moveVector.z / vectorLength;
        }
    }

    bool currentActionKey = (GetAsyncKeyState('J') & 0x8000) != 0;
    m_isActionTriggered = (currentActionKey && !m_previousActionKey);
    m_previousActionKey = currentActionKey;

    // ジャンプ
    const float GROUND_Y = 0.0f; // 一旦地面の高さを0

    if (playerPos.y <= GROUND_Y)
    {
        // 接地状態
        playerPos.y = GROUND_Y;
        m_velocityY = 0.0f;

        // ジャンプ入力
        if (GetAsyncKeyState(KEY_JUMP) & 0x8000) 
        {
            m_velocityY = JUMP_FORCE;
        }
    }
    else
    {
        // 空中状態（重力を適用）
        m_velocityY += GRAVITY * deltaTime;
    }

    // 速度を座標に反映
    playerPos.y += m_velocityY * deltaTime;

}

/*====================================================================
 * 視野切替キー（E）の押した瞬間を検出
 *====================================================================*/
bool Move::CheckFovToggle()
{
    static bool isPressed = false;
    if (GetAsyncKeyState(KEY_FOV_TOGGLE) & 0x8000)
    {
        if (!isPressed)
        {
            isPressed = true;
            return true;
        }
    }
    else
    {
        isPressed = false;
    }
    return false;
}

/*====================================================================
 * スタンキー（Q）の押した瞬間を検出
 *====================================================================*/
bool Move::CheckStunKey()
{
    static bool isPressed = false;
    if (GetAsyncKeyState(KEY_STUN) & 0x8000)
    {
        if (!isPressed)
        {
            isPressed = true;
            return true;
        }
    }
    else
    {
        isPressed = false;
    }
    return false;
}

/*====================================================================
 * 低速キー（R）の押した瞬間を検出
 *====================================================================*/
bool Move::CheckSlowKey()
{
    static bool isPressed = false;
    if (GetAsyncKeyState(KEY_SLOW) & 0x8000)
    {
        if (!isPressed)
        {
            isPressed = true;
            return true;
        }
    }
    else
    {
        isPressed = false;
    }
    return false;
}
