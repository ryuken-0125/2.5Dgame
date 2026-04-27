#pragma once
#include "Camera.h"
#include <DirectXMath.h>
#include <Windows.h>

class Move
{
public:
    Move();
    ~Move();

    // WASDでプレイヤーを動かす
    void ControlPlayer(DirectX::XMFLOAT3& playerPos, float deltaTime);

    // Eキーが押された瞬間だけ true を返す（トグル用）
    bool CheckFovToggle();

    // ※ControlCameraは追従になったため削除またはコメントアウトしてOKです
};