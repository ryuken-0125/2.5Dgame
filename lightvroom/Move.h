#pragma once
#include <DirectXMath.h>
#include <Windows.h>

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

private:
    // --- 移動定数 ---
    const float BASE_MOVE_SPEED = 5.0f;

    // --- キーバインド ---
    const int KEY_FOV_TOGGLE = 'E';
    const int KEY_STUN = 'Q';  // スタン発動キー
    const int KEY_SLOW = 'R';  // 低速発動キー
    const int KEY_FORWARD = 'W';
    const int KEY_BACK = 'S';
    const int KEY_RIGHT = 'D';
    const int KEY_LEFT = 'A';
};
