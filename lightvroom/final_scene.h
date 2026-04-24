#pragma once
#include "scene.h"
#include "game_context.h"
#include "Camera.h"
#include "tilemap.h"
#include "warp_zone.h"

class FinalScene : public Scene
{
public:
    FinalScene(SceneManager& sm, GameContext& ctx);

    void Initialize() override;
    void Finalize()   override;
    void Update(double deltaTime) override;
    void Draw()       override;

private:
    void HandleInput(float dt);
    void UpdatePhysics(float dt);
    void DrawScene();

    GameContext&       m_ctx;

    DirectX::XMFLOAT3  m_playerPos;
    DirectX::XMFLOAT3  m_playerVel;
    bool               m_onGround;
    bool               m_jumpSpaceWasDown;

    TileMap            m_tileMap;
    WarpZoneManager    m_exitZones;
    Camera             m_camera;

    float              m_elapsedTime;

    static const int MAP_DATA[TileMap::MAP_HEIGHT * TileMap::MAP_WIDTH];
};
