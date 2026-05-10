#pragma once
/*------------------------------
 * field_scene.h
 * 繝輔ぅ繝ｼ繝ｫ繝峨す繝ｼ繝ｳ・医・繝・・驕ｸ謚樒判髱｢・・ * 4 縺区園縺ｮ繝ｯ繝ｼ繝励だ繝ｼ繝ｳ繧呈戟縺｡縲√せ繝壹・繧ｹ繧ｭ繝ｼ縺ｧ遒ｺ隱坂・繧ｵ繝悶す繝ｼ繝ｳ縺ｸ驕ｷ遘ｻ
 *------------------------------*/

#include "scene.h"
#include "game_context.h"
#include "Camera.h"
#include "Move.h"
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
    GameContext&    m_ctx;
    Camera          m_camera;
    Move            m_move;
    WarpZoneManager m_warpZones;

    DirectX::XMFLOAT3 m_playerPos;
    bool              m_isWideCamera;

    // 譏ｼ螟懊し繧､繧ｯ繝ｫ
    float m_angle;

    void SetupWarpZones();
    void DrawScene(bool isShadowPass);
};
