
#include "Application.h"
#include <chrono>

Application::Application() : m_hwnd(nullptr), m_hInstance(nullptr), m_elapsedTime(0.0f), m_isWideCamera(false) {}

Application::~Application()
{
    Release();
}
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

    m_playerPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_camera.SetFOV(DirectX::XMConvertToRadians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
    m_elapsedTime = 0.0f;

    m_eyePos = DirectX::XMFLOAT3(0.0f, 15.0f, 15.0f); 
    m_lightTargetPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_lightMoveGoal = m_lightTargetPos;
    m_lightMoveTimer = 0.0f;

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

void Application::Release()
{
    m_playerTexture.reset();
    m_quadMesh.reset();
    m_floorMesh.reset();
    m_sphereMesh.reset();
    m_cubeMesh.reset();
    m_shadowMap.reset();
    m_shaderManager.reset();
    m_graphics.reset();
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

            Update(deltaTime); 
            Draw();            
            // Delegate to SceneManager
            m_sceneManager.Update(deltaTime);
            m_sceneManager.Draw();
        }
    }
}

void Application::Update(float deltaTime)
{
    m_elapsedTime += deltaTime;

    m_move.ControlPlayer(m_playerPos, deltaTime);

    if (m_move.CheckFovToggle()) {
        m_isWideCamera = !m_isWideCamera;
        float fov = m_isWideCamera ? 135.0f : 90.0f;
        m_camera.SetFOV(DirectX::XMConvertToRadians(fov), 1280.0f / 720.0f, 0.1f, 100.0f);
    }
    DirectX::XMFLOAT3 cameraOffset(0.0f, 4.0f, -6.0f);
    m_camera.SetFollowTarget(m_playerPos, cameraOffset);

    m_lightMoveTimer -= deltaTime;
    if (m_lightMoveTimer <= 0.0f) {
        m_lightMoveGoal.x = (float)(rand() % 200 - 100) / 10.0f;
        m_lightMoveGoal.z = (float)(rand() % 100 - 50) / 10.0f;
        m_lightMoveTimer = 2.0f + (rand() % 3);
    }

    m_lightTargetPos.x += (m_lightMoveGoal.x - m_lightTargetPos.x) * deltaTime * 1.5f;
    m_lightTargetPos.z += (m_lightMoveGoal.z - m_lightTargetPos.z) * deltaTime * 1.5f;

    using namespace DirectX;
    XMVECTOR eyeVec = XMLoadFloat3(&m_eyePos);
    XMVECTOR targetVec = XMLoadFloat3(&m_lightTargetPos);
    XMVECTOR spotDirVec = XMVector3Normalize(targetVec - eyeVec);

    m_currentSkyColor = XMFLOAT4(0.02f, 0.02f, 0.08f, 1.0f);

    m_frameData = {};
    m_frameData.viewProjection = XMMatrixTranspose(m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());
    m_frameData.cameraPos = m_camera.GetPosition();

    m_frameData.spotPos = m_eyePos;
    m_frameData.spotRange = 40.0f;
    XMStoreFloat3(&m_frameData.spotDir, spotDirVec);
    m_frameData.spotCosInner = cosf(XMConvertToRadians(10.0f)); 
    m_frameData.spotCosOuter = cosf(XMConvertToRadians(20.0f)); 
    m_frameData.spotColor = XMFLOAT3(8.0f, 8.0f, 10.0f); 
    m_frameData.skyColor = m_currentSkyColor;

    XMMATRIX lightView = XMMatrixLookAtLH(eyeVec, targetVec, XMVectorSet(0, 1, 0, 0));
    XMMATRIX lightProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 1.0f, 0.1f, 100.0f);
    m_frameData.lightViewProjection = XMMatrixTranspose(lightView * lightProj);
}
void Application::Draw()
{
    // ==========================================
    // ==========================================
    m_shadowMap->Bind(m_graphics->GetContext());
    m_shaderManager->BindShadowPass(m_graphics->GetContext(), m_playerTexture->GetSRV());
    m_shaderManager->UpdatePerFrame(m_graphics->GetContext(), m_frameData);
    DrawScene(true);


    m_graphics->SetMainRenderTarget();
    m_graphics->Clear(m_currentSkyColor.x, m_currentSkyColor.y, m_currentSkyColor.z, 1.0f);
    m_shaderManager->BindMainPass(m_graphics->GetContext(), m_shadowMap->GetSRV());

    DrawScene(false);

    using namespace DirectX;
    CBPerObject moonObj;
    moonObj.worldMatrix = XMMatrixTranspose(XMMatrixScaling(0.6f, 0.6f, 0.6f) * XMMatrixTranslationFromVector(XMLoadFloat3(&m_moonPos)));
    m_shaderManager->UpdatePerObject(m_graphics->GetContext(), moonObj);
    CBPerMaterial moonMat = { XMFLOAT4(0.8f, 0.8f, 1.0f, 1.0f), 0.0f, 0.0f, 3.0f, 0.0f };
    m_shaderManager->UpdatePerMaterial(m_graphics->GetContext(), moonMat);
    m_sphereMesh->Draw(m_graphics->GetContext());

    ID3D11ShaderResourceView* nullSRV = nullptr;
    m_graphics->GetContext()->PSSetShaderResources(0, 1, &nullSRV);

    m_graphics->Present();
}

void Application::DrawScene(bool isShadowPass)
{
    using namespace DirectX;

    CBPerObject floorObj;
    floorObj.worldMatrix = XMMatrixTranspose(XMMatrixScaling(40.0f, 0.1f, 40.0f) * XMMatrixTranslation(0.0f, -0.1f, 0.0f));
    m_shaderManager->UpdatePerObject(m_graphics->GetContext(), floorObj);
    CBPerMaterial floorMat = { XMFLOAT4(0.3f, 0.5f, 0.3f, 1.0f), 0.8f, 0.0f, 0.0f, 0.0f };
    m_shaderManager->UpdatePerMaterial(m_graphics->GetContext(), floorMat);
    m_floorMesh->Draw(m_graphics->GetContext());

    CBPerObject playerObj;
    XMMATRIX scale = XMMatrixScaling(1.5f, 1.5f, 1.5f);
    XMMATRIX rot = XMMatrixRotationX(XMConvertToRadians(30.0f));
    XMMATRIX trans = XMMatrixTranslation(m_playerPos.x, m_playerPos.y, m_playerPos.z);
    playerObj.worldMatrix = XMMatrixTranspose(scale * rot * trans);
    m_shaderManager->UpdatePerObject(m_graphics->GetContext(), playerObj);

    CBPerMaterial playerMat = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.9f, 0.0f, 0.0f, 1.0f };
    m_shaderManager->UpdatePerMaterial(m_graphics->GetContext(), playerMat);

    ID3D11ShaderResourceView* pSRV = m_playerTexture->GetSRV();
    if (!isShadowPass) m_graphics->GetContext()->PSSetShaderResources(1, 1, &pSRV);
    m_quadMesh->Draw(m_graphics->GetContext());

    ID3D11ShaderResourceView* nullSRV = nullptr;
    if (!isShadowPass) m_graphics->GetContext()->PSSetShaderResources(1, 1, &nullSRV);

    CBPerObject sphereObj;
    sphereObj.worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.0f, 0.5f, 2.0f));
    m_shaderManager->UpdatePerObject(m_graphics->GetContext(), sphereObj);
    CBPerMaterial sphereMat = { XMFLOAT4(0.56f, 0.57f, 0.58f, 1.0f), 0.1f, 1.0f, 0.0f, 0.0f };
    m_shaderManager->UpdatePerMaterial(m_graphics->GetContext(), sphereMat);
    m_sphereMesh->Draw(m_graphics->GetContext());
}

LRESULT CALLBACK Application::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
