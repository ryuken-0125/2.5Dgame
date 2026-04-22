#include "Move.h"

Move::Move() {}
Move::~Move() {}

void Move::ControlPlayer(DirectX::XMFLOAT3& playerPos, float deltaTime)
{
    float speed = 5.0f * deltaTime;

    // ===================================
    // WASDキーでプレイヤー(2Dキャラ)を移動
    // ===================================
    if (GetAsyncKeyState('W') & 0x8000) playerPos.z += speed; // 奥へ
    if (GetAsyncKeyState('S') & 0x8000) playerPos.z -= speed; // 手前へ
    if (GetAsyncKeyState('D') & 0x8000) playerPos.x += speed; // 右へ
    if (GetAsyncKeyState('A') & 0x8000) playerPos.x -= speed; // 左へ
}

bool Move::CheckFovToggle()
{
    // static変数を使って、キーを「押しっぱなし」にしても1回しか反応しないようにする
    static bool isEPressed = false;

    if (GetAsyncKeyState('E') & 0x8000)
    {
        if (!isEPressed) {
            isEPressed = true;
            return true; // 押した瞬間！
        }
    }
    else
    {
        isEPressed = false; // 離した
    }
    return false;
}