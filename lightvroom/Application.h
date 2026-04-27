#pragma once
#include <Windows.h>
#include <memory>

#include "Graphics.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "ShadowMap.h"
#include "Texture.h"

#include "PlayerManager.h"
#include "lightManager.h"
#include "Camera.h"
#include "warp_zone.h"
#include "game_context.h"
#include "scene_manager.h"

class Application
{
public:
    Application();
    ~Application();

    bool Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height);
    void Run();
    void Finalize();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void Update(float deltaTime, float elapsedTime);
    void Draw();

    HWND      m_hwnd;
    HINSTANCE m_hInstance;

    std::unique_ptr<Graphics>      m_graphics;
    std::unique_ptr<ShaderManager> m_shaderManager;
    std::unique_ptr<ShadowMap>     m_shadowMap;

    std::unique_ptr<Mesh> m_cubeMesh;
    std::unique_ptr<Mesh> m_sphereMesh;
    std::unique_ptr<Mesh> m_floorMesh;
    std::unique_ptr<Mesh> m_quadMesh;

    std::unique_ptr<PlayerManager> m_playerManager;
    std::unique_ptr<LightManager>  m_lightManager;

    GameContext m_gameContext;
    SceneManager m_sceneManager;

    Camera m_camera;
    WarpZoneManager m_warpZones;
    bool   m_isWideCamera = false;
};
