#pragma once
#include <Windows.h>
#include <memory>

#include "Graphics.h"
#include "ShaderManager.h" 
#include "Mesh.h"
#include "ShadowMap.h"
#include "Texture.h"
#include "game_context.h"
#include "scene_manager.h"

class Application
{
public:
    Application();
    ~Application();

    bool Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height);
    void Run();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd;
    HINSTANCE m_hInstance;

    std::unique_ptr<Graphics> m_graphics;
    std::unique_ptr<ShaderManager> m_shaderManager; 

    std::unique_ptr<Mesh> m_cubeMesh;
    std::unique_ptr<Mesh> m_sphereMesh;
    std::unique_ptr<Mesh> m_floorMesh;
    std::unique_ptr<Mesh> m_quadMesh;

    std::unique_ptr<ShadowMap> m_shadowMap;
    std::unique_ptr<Texture> m_playerTexture;
    std::unique_ptr<Texture> m_tilesetTexture;

    GameContext m_gameContext;
    SceneManager m_sceneManager;
};
