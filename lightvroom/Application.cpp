#include "Application.h"
#include <DirectXMath.h>
#include <chrono>


Application::Application() : m_hwnd(nullptr), m_hInstance(nullptr) {}
Application::~Application() {}

bool Application::Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height)
{
    m_hInstance = hInstance;
    const char CLASS_NAME[] = "WindowClass";
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    if (!RegisterClass(&wc)) return false;

    RECT wr = { 0, 0, width, height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindowEx(0, CLASS_NAME, "2.5D(DX11)", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, hInstance, nullptr);
    if (m_hwnd == nullptr) return false;

    ShowWindow(m_hwnd, nCmdShow);

    m_graphics = std::make_unique<Graphics>();
    if (!m_graphics->Initialize(m_hwnd, width, height)) return false;

    m_shaderManager = std::make_unique<ShaderManager>();
    if (!m_shaderManager->Initialize(m_graphics->GetDevice(), L"StandardPBR.hlsl", L"Shadow.hlsl")) return false;

    m_shadowMap = std::make_unique<ShadowMap>();
    if (!m_shadowMap->Initialize(m_graphics->GetDevice(), 2048, 2048)) return false;

    m_cubeMesh = std::make_unique<Mesh>();
    m_cubeMesh->CreateCube(m_graphics->GetDevice());

    m_sphereMesh = std::make_unique<Mesh>();
    m_sphereMesh->CreateSphere(m_graphics->GetDevice(), 1.0f, 30, 30);

    m_floorMesh = std::make_unique<Mesh>();
    m_floorMesh->CreateCube(m_graphics->GetDevice());

    m_quadMesh = std::make_unique<Mesh>();
    m_quadMesh->CreateQuad(m_graphics->GetDevice()); // 2D Sprite

    m_playerTexture = std::make_unique<Texture>();
    if (!m_playerTexture->Load(m_graphics->GetDevice(), "../asset/texture/player.png")) {
        MessageBox(m_hwnd, "player.png load failed!", "Error", MB_OK);
        return false;
    }

    m_tilesetTexture = std::make_unique<Texture>();
    if (!m_tilesetTexture->Load(m_graphics->GetDevice(), "../asset/texture/tileset.png")) {
        MessageBox(m_hwnd, "tileset.png load failed!", "Error", MB_OK);
        return false;
    }

    // Set up GameContext
    m_gameContext.graphics = m_graphics.get();
    m_gameContext.shaderManager = m_shaderManager.get();
    m_gameContext.quadMesh = m_quadMesh.get();
    m_gameContext.cubeMesh = m_cubeMesh.get();
    m_gameContext.floorMesh = m_floorMesh.get();
    m_gameContext.sphereMesh = m_sphereMesh.get();
    m_gameContext.shadowMap = m_shadowMap.get();
    m_gameContext.playerTexture = m_playerTexture.get();
    m_gameContext.tilesetTexture = m_tilesetTexture.get();

    // Initialize SceneManager
    m_sceneManager.Init(&m_gameContext);
    m_sceneManager.ChangeScene(SceneType::FIELD);

    return true;

}

void Application::Run()
{
    MSG msg = { };
    auto startTime = std::chrono::high_resolution_clock::now();
    auto prevTime = startTime;

    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - prevTime).count();
            prevTime = currentTime;

            // Delegate to SceneManager
            m_sceneManager.Update(deltaTime);
            m_sceneManager.Draw();
        }
    }
}

LRESULT CALLBACK Application::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
