#pragma once
#include <Windows.h>
#include <memory>

// 管理クラスのみをインクルード
#include "Graphics.h"
#include "ShaderManager.h" 
#include "scene_manager.h"
#include "game_context.h" // リソース共有用構造体

class Application
{
public:
    Application();
    ~Application();

    // アプリケーションの初期化
    bool Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height);

    // メインループの実行
    void Run();

private:
    // ウィンドウメッセージ処理
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND      m_hwnd;
    HINSTANCE m_hInstance;

    // 基盤システムの保持
    std::unique_ptr<Graphics>      m_graphics;
    std::unique_ptr<ShaderManager> m_shaderManager;

    // シーン管理（ゲーム本編はここから下の階層で管理）
    std::unique_ptr<SceneManager>  m_sceneManager;

    // 各クラスで共有する共通データ（Graphicsのポインタなど）
    std::shared_ptr<GameContext>   m_gameContext;
};