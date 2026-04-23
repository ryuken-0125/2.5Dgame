
#include "Application.h"
#include <chrono>

Application::Application() : m_hwnd(nullptr), m_hInstance(nullptr), m_elapsedTime(0.0f), m_isWideCamera(false) {}

Application::~Application()
{
    Release(); // デストラクタで確実に解放処理を呼ぶ
}

bool Application::Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height)
{
    // --- ウィンドウとDirectXの初期化 ---
    m_hInstance = hInstance;
    const wchar_t CLASS_NAME[] = L"WindowClass";
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    if (!RegisterClass(&wc)) return false;

    RECT wr = { 0, 0, width, height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindowEx(0, CLASS_NAME, L"2.5Dgame(DX11)", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, hInstance, nullptr);
    if (m_hwnd == nullptr) return false;

    ShowWindow(m_hwnd, nCmdShow);

    // --- 各種エンジンの初期化 ---
    m_graphics = std::make_unique<Graphics>();
    if (!m_graphics->Initialize(m_hwnd, width, height)) return false;

    m_shaderManager = std::make_unique<ShaderManager>();
    if (!m_shaderManager->Initialize(m_graphics->GetDevice(), L"StandardPBR.hlsl", L"Shadow.hlsl")) return false;

    m_shadowMap = std::make_unique<ShadowMap>();
    if (!m_shadowMap->Initialize(m_graphics->GetDevice(), 2048, 2048)) return false;

    // --- メッシュとテクスチャの準備 ---
    m_cubeMesh = std::make_unique<Mesh>();
    m_cubeMesh->CreateCube(m_graphics->GetDevice());
    m_sphereMesh = std::make_unique<Mesh>();
    m_sphereMesh->CreateSphere(m_graphics->GetDevice(), 1.0f, 30, 30);
    m_floorMesh = std::make_unique<Mesh>();
    m_floorMesh->CreateCube(m_graphics->GetDevice());
    m_quadMesh = std::make_unique<Mesh>();
    m_quadMesh->CreateQuad(m_graphics->GetDevice());

    m_playerTexture = std::make_unique<Texture>();
    if (!m_playerTexture->Load(m_graphics->GetDevice(), "../asset/texture/player.png")) {
        MessageBox(m_hwnd, L"player.png の読み込みに失敗しました！", L"エラー", MB_OK);
        return false;
    }

    // --- 初期状態のセット ---
    m_playerPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_camera.SetFOV(DirectX::XMConvertToRadians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
    m_elapsedTime = 0.0f;

    m_eyePos = DirectX::XMFLOAT3(0.0f, 15.0f, 15.0f); // ステージ中央奥の高い位置
    m_lightTargetPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_lightMoveGoal = m_lightTargetPos;
    m_lightMoveTimer = 0.0f;

    return true;
}

void Application::Release()
{
    // メモリを明示的に掃除する（シーン切り替え時などにも使える非常に良い設計です）
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

    // ループ内が極限までスッキリしました！
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

            Update(deltaTime); // 1. 頭で考える
            Draw();            // 2. 絵を描く
        }
    }
}

void Application::Update(float deltaTime)
{
    m_elapsedTime += deltaTime;

    // --- 1. 入力処理とキャラクター移動 ---
    m_move.ControlPlayer(m_playerPos, deltaTime);

    // --- 2. カメラの更新 ---
    if (m_move.CheckFovToggle()) {
        m_isWideCamera = !m_isWideCamera;
        float fov = m_isWideCamera ? 135.0f : 90.0f;
        m_camera.SetFOV(DirectX::XMConvertToRadians(fov), 1280.0f / 720.0f, 0.1f, 100.0f);
    }
    DirectX::XMFLOAT3 cameraOffset(0.0f, 4.0f, -6.0f);
    m_camera.SetFollowTarget(m_playerPos, cameraOffset);

    // --- 3. 光の目標地点をランダムに更新 ---
    m_lightMoveTimer -= deltaTime;
    if (m_lightMoveTimer <= 0.0f) {
        // 次の目標地点をランダムに決定（X:-10～10, Z:-5～5 程度）
        m_lightMoveGoal.x = (float)(rand() % 200 - 100) / 10.0f;
        m_lightMoveGoal.z = (float)(rand() % 100 - 50) / 10.0f;
        m_lightMoveTimer = 2.0f + (rand() % 3); // 2～5秒ごとに更新
    }

    // 目標地点へゆっくり移動
    m_lightTargetPos.x += (m_lightMoveGoal.x - m_lightTargetPos.x) * deltaTime * 1.5f;
    m_lightTargetPos.z += (m_lightMoveGoal.z - m_lightTargetPos.z) * deltaTime * 1.5f;

    // --- 4. スポットライトの方向を計算 ---
    using namespace DirectX;
    XMVECTOR eyeVec = XMLoadFloat3(&m_eyePos);
    XMVECTOR targetVec = XMLoadFloat3(&m_lightTargetPos);
    XMVECTOR spotDirVec = XMVector3Normalize(targetVec - eyeVec);

    // 常に「夜」の状態
    m_currentSkyColor = XMFLOAT4(0.02f, 0.02f, 0.08f, 1.0f);

    // --- 5. GPUへ送る荷物（定数バッファ）の準備 ---
    m_frameData = {};
    m_frameData.viewProjection = XMMatrixTranspose(m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());
    m_frameData.cameraPos = m_camera.GetPosition();

    // スポットライトのデータを定数バッファに詰める
    m_frameData.spotPos = m_eyePos;
    m_frameData.spotRange = 40.0f;
    XMStoreFloat3(&m_frameData.spotDir, spotDirVec);
    m_frameData.spotCosInner = cosf(XMConvertToRadians(10.0f)); // 10度までは全開
    m_frameData.spotCosOuter = cosf(XMConvertToRadians(20.0f)); // 20度で消える
    m_frameData.spotColor = XMFLOAT3(8.0f, 8.0f, 10.0f); // 強い青白い光
    m_frameData.skyColor = m_currentSkyColor;

    // ★ここで影用カメラを「スポットライト（パース）」として1回だけ定義する★
    XMMATRIX lightView = XMMatrixLookAtLH(eyeVec, targetVec, XMVectorSet(0, 1, 0, 0));
    // ※広角レンズのように影を落とすため、PerspectiveFovLH を使います
    XMMATRIX lightProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 1.0f, 0.1f, 100.0f);
    m_frameData.lightViewProjection = XMMatrixTranspose(lightView * lightProj);
}
void Application::Draw()
{
    // ==========================================
    // パス1：シャドウマップ（影）の描画
    // ==========================================
    m_shadowMap->Bind(m_graphics->GetContext());
    m_shaderManager->BindShadowPass(m_graphics->GetContext(), m_playerTexture->GetSRV());
    m_shaderManager->UpdatePerFrame(m_graphics->GetContext(), m_frameData);
    DrawScene(true);

    // ==========================================
    // パス2：メイン画面（本番）の描画
    // ==========================================
    m_graphics->SetMainRenderTarget();
    m_graphics->Clear(m_currentSkyColor.x, m_currentSkyColor.y, m_currentSkyColor.z, 1.0f);
    m_shaderManager->BindMainPass(m_graphics->GetContext(), m_shadowMap->GetSRV());

    DrawScene(false);

    // --- 月の描画 ---
    using namespace DirectX;
    CBPerObject moonObj;
    moonObj.worldMatrix = XMMatrixTranspose(XMMatrixScaling(0.6f, 0.6f, 0.6f) * XMMatrixTranslationFromVector(XMLoadFloat3(&m_moonPos)));
    m_shaderManager->UpdatePerObject(m_graphics->GetContext(), moonObj);
    CBPerMaterial moonMat = { XMFLOAT4(0.8f, 0.8f, 1.0f, 1.0f), 0.0f, 0.0f, 3.0f, 0.0f };
    m_shaderManager->UpdatePerMaterial(m_graphics->GetContext(), moonMat);
    m_sphereMesh->Draw(m_graphics->GetContext());

    // --- 後処理 ---
    ID3D11ShaderResourceView* nullSRV = nullptr;
    m_graphics->GetContext()->PSSetShaderResources(0, 1, &nullSRV);

    m_graphics->Present();
}

void Application::DrawScene(bool isShadowPass)
{
    using namespace DirectX;

    // 1. 地面
    CBPerObject floorObj;
    floorObj.worldMatrix = XMMatrixTranspose(XMMatrixScaling(40.0f, 0.1f, 40.0f) * XMMatrixTranslation(0.0f, -0.1f, 0.0f));
    m_shaderManager->UpdatePerObject(m_graphics->GetContext(), floorObj);
    CBPerMaterial floorMat = { XMFLOAT4(0.3f, 0.5f, 0.3f, 1.0f), 0.8f, 0.0f, 0.0f, 0.0f };
    m_shaderManager->UpdatePerMaterial(m_graphics->GetContext(), floorMat);
    m_floorMesh->Draw(m_graphics->GetContext());

    // 2. プレイヤー(2Dの板)
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

    // 3. 鉄の球体
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