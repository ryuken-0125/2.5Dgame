
#pragma once
#include <windows.h>
#include <memory>
#include "Graphics.h"
#include "ShaderManager.h"
#include "ShadowMap.h"
#include "Mesh.h"
#include "ShadowMap.h"
#include "Texture.h"
#include "game_context.h"
#include "scene_manager.h"
#include "Move.h"
#include "Effect.h"


class Application
{
public:
    Application();
    ~Application();

    bool Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height);
    void Run();
    void Release(); // ★追加：明示的な解放処理

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // ★追加：ループの中身を役割ごとに分割
    void Update(float deltaTime);
    void Draw();
    void DrawScene(bool isShadowPass);

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

    std::unique_ptr<Move> m_move;                 ///< 入力・移動管理
    std::unique_ptr<Effect> m_effect;             ///< エフェクト管理  

    GameContext m_gameContext;
    SceneManager m_sceneManager;
};
