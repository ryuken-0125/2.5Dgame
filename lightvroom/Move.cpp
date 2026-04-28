#include "Move.h"
#include "PlayerStatus.h"

Move::Move() {}
Move::~Move() {}

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

    // 低速状態なら速度倍率を適用
    float speed = BASE_MOVE_SPEED * status.GetSpeedMultiplier() * deltaTime;

    if (GetAsyncKeyState(KEY_FORWARD) & 0x8000) playerPos.z += speed;  // 奥へ
    if (GetAsyncKeyState(KEY_BACK) & 0x8000) playerPos.z -= speed;  // 手前へ
    if (GetAsyncKeyState(KEY_RIGHT) & 0x8000) playerPos.x += speed;  // 右へ
    if (GetAsyncKeyState(KEY_LEFT) & 0x8000) playerPos.x -= speed;  // 左へ
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
