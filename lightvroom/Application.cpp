#include "Application.h"
#include "game_context.h"
#include "scene_manager.h"
#include <DirectXMath.h>
#include <chrono>

Application::Application() : m_hwnd(nullptr), m_hInstance(nullptr) {}
Application::~Application() {}

bool Application::Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height) {
    m_hInstance = hInstance;

    // 1. ウィンドウクラスの登録
    const char CLASS_NAME[] = "WindowClass";
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    if (!RegisterClass(&wc)) return false;

    // 2. ウィンドウの作成
    RECT wr = { 0, 0, width, height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    m_hwnd = CreateWindowEx(0, CLASS_NAME, "2.5D Game Engine", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, hInstance, nullptr);

    if (m_hwnd == nullptr) return false;
    ShowWindow(m_hwnd, nCmdShow);

    // 3. 基盤システムの初期化
    m_graphics = std::make_unique<Graphics>();
    if (!m_graphics->Initialize(m_hwnd, width, height)) return false;

    m_shaderManager = std::make_unique<ShaderManager>();
    if (!m_shaderManager->Initialize(m_graphics->GetDevice())) return false;

    // 4. GameContext（共通リソース）のセットアップ
    m_gameContext = std::make_shared<GameContext>();
    m_gameContext->device = m_graphics->GetDevice();
    m_gameContext->context = m_graphics->GetContext();
    m_gameContext->shaderManager = m_shaderManager.get();

    // 5. シーン管理の初期化（ここで最初のシーンが生成される）
    m_sceneManager = std::make_unique<SceneManager>(m_gameContext);

    return true;
}

void Application::Run() {
    MSG msg = { };
    auto startTime = std::chrono::high_resolution_clock::now();
    auto prevTime = startTime;

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {

            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - prevTime).count();
            prevTime = currentTime;
            // --- アップデート ---
            m_sceneManager->Update(deltaTime);

            // シーンの描画実行
            m_sceneManager->Draw();
        }
    }
}

// ウィンドウプロシージャ（最小限の記述）
LRESULT CALLBACK Application::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}