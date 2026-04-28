#pragma once
/*------------------------------
 * field_scene.h
 * フィールドシーン（マップ選択画面）
 * 4つのワープゾーンを持ち、スペースキーで確認後サブシーンへ遷移
 *------------------------------*/
#include "scene.h"
#include "game_context.h"
#include "Camera.h"
#include "Move.h"
#include "PlayerStatus.h"   // 追加
#include "warp_zone.h"
#include <DirectXMath.h>
#include <chrono>

class FieldScene : public Scene
{
public:
    FieldScene(SceneManager& sm, GameContext& ctx);
    ~FieldScene() override = default;
    void Initialize() override;
    void Finalize()   override;
    void Update(double deltaTime) override;
    void Draw()       override;

private:
    GameContext& m_ctx;
    Camera            m_camera;
    Move              m_move;
    PlayerStatus      m_playerStatus;   // 追加
    WarpZoneManager   m_warpZones;

    DirectX::XMFLOAT3 m_playerPos;
    bool              m_isWideCamera;

    // 昼夜サイクル
    float m_angle;

    void SetupWarpZones();
    void DrawScene(bool isShadowPass);
};
