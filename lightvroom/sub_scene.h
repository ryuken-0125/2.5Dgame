#pragma once
/*------------------------------
 * sub_scene.h
 * Action sub-scene (2D side-scrolling tilemap)
 *------------------------------*/

#include "scene.h"
#include "game_context.h"
#include "camera.h"
#include "tilemap.h"
#include "warp_zone.h"
#include "move.h"

class SubScene : public Scene
{
public:
    SubScene(SceneManager& sm, GameContext& ctx, int index);

    void Initialize() override;
    void Finalize()   override;
    void Update(double deltaTime) override;
    void Draw()       override;

private:
    void LoadMap();
    void SetupExitZone();
    void HandleInput(float dt);
    void UpdatePhysics(float dt);
    void DrawScene();

    GameContext&     m_ctx;
    int              m_index;

    DirectX::XMFLOAT3 m_playerPos;
    DirectX::XMFLOAT3 m_playerVel;
    bool              m_onGround;
    bool              m_jumpSpaceWasDown;

    TileMap          m_tileMap;
    WarpZoneManager  m_exitZones;
    Camera           m_camera;

    static const int MAP_DATA_0[TileMap::MAP_HEIGHT * TileMap::MAP_WIDTH];
    static const int MAP_DATA_1[TileMap::MAP_HEIGHT * TileMap::MAP_WIDTH];
    static const int MAP_DATA_2[TileMap::MAP_HEIGHT * TileMap::MAP_WIDTH];
    static const int MAP_DATA_3[TileMap::MAP_HEIGHT * TileMap::MAP_WIDTH];
};