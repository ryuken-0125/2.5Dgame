#include "Application.h"
#include <chrono>
#include <string>

Application::Application() : m_hwnd(nullptr), m_hInstance(nullptr) {}
Application::~Application() { Finalize(); }

// Helper: show an error box and return false
static bool Fail(HWND hwnd, const char* msg)
{
    MessageBox(hwnd, msg, "Initialization Error", MB_OK | MB_ICONERROR);
    return false;
}

bool Application::Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height)
{
    m_hInstance = hInstance;
    const char CLASS_NAME[] = "WindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    if (!RegisterClass(&wc)) return Fail(nullptr, "RegisterClass failed");

    RECT wr = { 0, 0, width, height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindowEx(0, CLASS_NAME, "2.5D (DX11)", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, hInstance, nullptr);
    if (!m_hwnd) return Fail(nullptr, "CreateWindow failed");

    ShowWindow(m_hwnd, nCmdShow);

    // Graphics
    m_graphics = std::make_unique<Graphics>();
    if (!m_graphics->Initialize(m_hwnd, width, height))
        return Fail(m_hwnd, "Graphics::Initialize failed\nD3D11 device creation error.");

    // Resolve HLSL paths relative to the exe's directory
    char exePath[MAX_PATH] = {};
    GetModuleFileName(nullptr, exePath, MAX_PATH);
    std::string pathStr = exePath;
    size_t lastSlash = pathStr.find_last_of("\\/");
    std::string exeDir = (lastSlash != std::string::npos) ? pathStr.substr(0, lastSlash + 1) : "";

    std::string pbrPath    = exeDir + "StandardPBR.hlsl";
    std::string shadowPath = exeDir + "Shadow.hlsl";

    // ShaderManager
    m_shaderManager = std::make_unique<ShaderManager>();
    if (!m_shaderManager->Initialize(m_graphics->GetDevice(), pbrPath, shadowPath))
        return Fail(m_hwnd,
            "ShaderManager::Initialize failed\n"
            "Make sure StandardPBR.hlsl and Shadow.hlsl are copied next to the exe.");

    // ShadowMap
    m_shadowMap = std::make_unique<ShadowMap>();
    if (!m_shadowMap->Initialize(m_graphics->GetDevice(), 2048, 2048))
        return Fail(m_hwnd, "ShadowMap::Initialize failed");

    // Meshes
    m_cubeMesh = std::make_unique<Mesh>();
    m_cubeMesh->CreateCube(m_graphics->GetDevice());

    m_sphereMesh = std::make_unique<Mesh>();
    m_sphereMesh->CreateSphere(m_graphics->GetDevice(), 1.0f, 30, 30);

    m_floorMesh = std::make_unique<Mesh>();
    m_floorMesh->CreateCube(m_graphics->GetDevice());

    m_quadMesh = std::make_unique<Mesh>();
    m_quadMesh->CreateQuad(m_graphics->GetDevice());

    // LightManager
    m_lightManager = std::make_unique<LightManager>();

    // PlayerManager
    m_playerManager = std::make_unique<PlayerManager>();
    if (!m_playerManager->Initialize(m_graphics->GetDevice()))
        return Fail(m_hwnd, "PlayerManager::Initialize failed");

    // Warp Zones
    m_warpZones.AddZone(
        AABB::Make(DirectX::XMFLOAT3(5.0f, 0.5f, 5.0f), DirectX::XMFLOAT3(3.0f, 1.0f, 3.0f)),
        1, "Secret Stage", DirectX::XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f)
    );

    // Camera
    m_camera.SetPosition(0.0f, 2.0f, -8.0f);
    m_camera.SetFOV(DirectX::XMConvertToRadians(90.0f), (float)width / (float)height, 0.1f, 100.0f);

    // Fill GameContext
    m_gameContext.graphics      = m_graphics.get();
    m_gameContext.shaderManager = m_shaderManager.get();
    m_gameContext.lightManager  = m_lightManager.get();
    m_gameContext.shadowMap     = m_shadowMap.get();
    m_gameContext.quadMesh      = m_quadMesh.get();
    m_gameContext.cubeMesh      = m_cubeMesh.get();
    m_gameContext.floorMesh     = m_floorMesh.get();
    m_gameContext.sphereMesh    = m_sphereMesh.get();
    m_gameContext.playerTexture = m_playerManager->GetTexture();

    // SceneManager
    m_sceneManager.Init(&m_gameContext);
    m_sceneManager.ChangeScene(SceneType::FIELD);

    return true;
}

void Application::Finalize()
{
    m_playerManager.reset();
    m_quadMesh.reset();
    m_floorMesh.reset();
    m_sphereMesh.reset();
    m_cubeMesh.reset();
    m_lightManager.reset();
    m_shadowMap.reset();
    m_shaderManager.reset();
    m_graphics.reset();
}

void Application::Update(float deltaTime, float elapsedTime)
{
    m_sceneManager.Update(deltaTime);
}

void Application::Draw()
{
    m_sceneManager.Draw();

    ID3D11ShaderResourceView* nullSRV = nullptr;
    m_gameContext.graphics->GetContext()->PSSetShaderResources(0, 1, &nullSRV);
    m_gameContext.graphics->Present();
}

void Application::Run()
{
    MSG msg = {};
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
            if (deltaTime > 0.1f) deltaTime = 0.1f;
            prevTime = currentTime;
            float elapsedTime = std::chrono::duration<float>(currentTime - startTime).count();

            Update(deltaTime, elapsedTime);
            Draw();
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
