#include "Application.h"
#include <DirectXMath.h>
#include <chrono>

Application::Application() : m_hwnd(nullptr), m_hInstance(nullptr) {}
Application::~Application() {}

bool Application::Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height)
{
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

    m_hwnd = CreateWindowEx(0, CLASS_NAME, L"LightVroom(DX11)", WS_OVERLAPPEDWINDOW,
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

    m_camera.SetPosition(0.0f, 2.0f, -8.0f);
    m_camera.SetProjection(DirectX::XMConvertToRadians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    m_quadMesh = std::make_unique<Mesh>();
    m_quadMesh->CreateQuad(m_graphics->GetDevice()); // 2D用板を作成

    m_playerPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f); // 真ん中からスタート
    m_isWideCamera = false; // 最初は標準カメラ

    // 標準の画角(45度)をセット
    m_camera.SetFOV(DirectX::XMConvertToRadians(90.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    // テクスチャの読み込み（相対パスを指定）
    m_playerTexture = std::make_unique<Texture>();
    if (!m_playerTexture->Load(m_graphics->GetDevice(), "../asset/texture/player.png")) {
        MessageBox(m_hwnd, L"player.png の読み込みに失敗しました！", L"エラー", MB_OK);
        return false;
    }

    return true;

}

void Application::Run()
{
    MSG msg = { };
    auto startTime = std::chrono::high_resolution_clock::now();
    auto prevTime = startTime;

    const float DAY_DURATION = 10.0f; // 1日の長さ（10秒）

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
            float elapsedTime = std::chrono::duration<float>(currentTime - startTime).count();

            // --- 入力の処理と更新 ---
    
            // 1. WASDでプレイヤーを動かす
            m_move.ControlPlayer(m_playerPos, deltaTime);

            // 2. Eキーでカメラ切り替え
            if (m_move.CheckFovToggle())
            {
                m_isWideCamera = !m_isWideCamera; // 状態を反転
                float fov = m_isWideCamera ? 135.0f : 90.0f; // 広角:135度, 標準:90度
                m_camera.SetFOV(DirectX::XMConvertToRadians(fov), 1280.0f / 720.0f, 0.1f, 100.0f);
            }

            // 3. カメラをプレイヤーに追従させる（斜め上から見下ろす2.5Dの定番アングル）
            DirectX::XMFLOAT3 cameraOffset(0.0f, 4.0f, -6.0f);
            m_camera.SetFollowTarget(m_playerPos, cameraOffset);

            // --- 1日のサイクル計算（西から東へ） ---
            float dayTime = fmodf(elapsedTime / DAY_DURATION, 1.0f);
            float angle = dayTime * DirectX::XM_2PI; // 0 ～ 2PI

            using namespace DirectX;

            // 太陽の位置 (XY平面上で円を描く)
            XMVECTOR sunPos = XMVectorSet(cosf(angle) * 25.0f, sinf(angle) * 25.0f, 0.0f, 0.0f);
            XMVECTOR sunDir = XMVector3Normalize(-sunPos); // 物体へ向かう方向

            // 月の位置 (太陽の正反対)
            XMVECTOR moonPos = -sunPos;
            XMVECTOR moonDir = XMVector3Normalize(-moonPos);

            // --- 時間帯に応じた色の設定 ---
            XMFLOAT4 currentSkyColor;
            XMFLOAT3 currentSunColor;

            /* === 太陽の光と空の変化をコメントアウト ===
            float sunIntensity = max(0.0f, sinf(angle));
            if (sunIntensity > 0.0f) {
                currentSkyColor = XMFLOAT4(0.2f, 0.4f, 0.8f, 1.0f);
                currentSunColor = XMFLOAT3(5.0f * sunIntensity, 4.5f * sunIntensity, 4.0f * sunIntensity);
            } else {
                currentSkyColor = XMFLOAT4(0.02f, 0.02f, 0.1f, 1.0f);
                currentSunColor = XMFLOAT3(0.0f, 0.0f, 0.0f);
            }
            ========================================== */

            // 常に「夜」の状態に固定する
            currentSkyColor = XMFLOAT4(0.1f, 0.1f, 0.4f, 1.0f); // 漆黒に近い、わずかに青い闇
            currentSunColor = XMFLOAT3(0.0f, 0.0f, 0.0f);          // 太陽の光は完全にゼロ

            float moonIntensity = max(0.0f, sinf(angle + DirectX::XM_PI));

            // 月の光
            XMFLOAT3 currentMoonColor = XMFLOAT3(0.7f * moonIntensity, 0.8f * moonIntensity, 1.2f * moonIntensity);

            // ==========================================
            //定数バッファの準備
            // ==========================================
            CBPerFrame frameData = {};
            frameData.viewProjection = XMMatrixTranspose(m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix());
            frameData.cameraPos = m_camera.GetPosition();

            // 太陽の光の強さが 0 より大きい（昼）なら太陽の向き、そうでない（夜）なら月の向きを使う
            //影用カメラの太陽・月切り替えをコメントアウト
            //XMVECTOR activeLightDir = (sunIntensity > 0.0f) ? sunDir : moonDir;

            // 影のカメラ（光源）は常に「月」に固定する
            XMVECTOR activeLightDir = moonDir;

            // ========================================================
            // 影用のカメラを、プレイヤーに追従するように変更
            // ========================================================
            XMVECTOR playerVec = XMLoadFloat3(&m_playerPos);
            // プレイヤーの位置から、光の差す方向と逆（上空）へカメラを移動させる
            XMVECTOR lightPosForShadow = playerVec + activeLightDir * -30.0f;

            // 常にプレイヤー(playerVec)を見つめるようにする
            XMMATRIX lightView = XMMatrixLookAtLH(lightPosForShadow, playerVec, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));

            // 念のため影の描画範囲を 30 から 40 に広げておく
            XMMATRIX lightProj = XMMatrixOrthographicLH(40.0f, 40.0f, 0.1f, 100.0f);
            frameData.lightViewProjection = XMMatrixTranspose(lightView * lightProj);
            // ========================================================

            XMStoreFloat3(&frameData.sunDir, sunDir);
            frameData.sunColor = currentSunColor;
            XMStoreFloat3(&frameData.moonDir, moonDir);
            frameData.moonColor = currentMoonColor;
            frameData.skyColor = currentSkyColor;


            // ==========================================
            // --- 描画関数 ---
            // ==========================================
            auto DrawScene = [&](bool isShadowPass) {
                // 1. 地面
                CBPerObject floorObj;
                // ※移動範囲を広げるため、地面のサイズを 15->40 に拡大しておきます
                floorObj.worldMatrix = XMMatrixTranspose(XMMatrixScaling(40.0f, 0.1f, 40.0f) * XMMatrixTranslation(0.0f, -0.1f, 0.0f));
                m_shaderManager->UpdatePerObject(m_graphics->GetContext(), floorObj);

                //if (!isShadowPass) を削除！影のパスでも材質を更新する
                CBPerMaterial floorMat = { XMFLOAT4(0.3f, 0.5f, 0.3f, 1.0f), 0.8f, 0.0f, 0.0f, 0.0f }; // useTexture = 0.0
                m_shaderManager->UpdatePerMaterial(m_graphics->GetContext(), floorMat);

                m_floorMesh->Draw(m_graphics->GetContext());

                // 2. プレイヤー(2Dの板)
                CBPerObject playerObj;
                DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(1.5f, 1.5f, 1.5f);
                DirectX::XMMATRIX rot = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(30.0f));
                DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(m_playerPos.x, m_playerPos.y, m_playerPos.z);
                playerObj.worldMatrix = XMMatrixTranspose(scale * rot * trans);
                m_shaderManager->UpdatePerObject(m_graphics->GetContext(), playerObj);

                //if (!isShadowPass) を削除！
                CBPerMaterial playerMat = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.9f, 0.0f, 0.0f, 1.0f }; // useTexture = 1.0
                m_shaderManager->UpdatePerMaterial(m_graphics->GetContext(), playerMat);

                ID3D11ShaderResourceView* pSRV = m_playerTexture->GetSRV();
                if (!isShadowPass) m_graphics->GetContext()->PSSetShaderResources(1, 1, &pSRV);

                m_quadMesh->Draw(m_graphics->GetContext());

                ID3D11ShaderResourceView* nullSRV = nullptr;
                if (!isShadowPass) m_graphics->GetContext()->PSSetShaderResources(1, 1, &nullSRV);

                // 3. 鉄の球体
                CBPerObject sphereObj;
                sphereObj.worldMatrix = XMMatrixTranspose(XMMatrixTranslation(2.0f, 1.0f, 2.0f));
                m_shaderManager->UpdatePerObject(m_graphics->GetContext(), sphereObj);

                //if (!isShadowPass) を削除！
                CBPerMaterial sphereMat = { XMFLOAT4(0.56f, 0.57f, 0.58f, 1.0f), 0.1f, 1.0f, 0.0f, 0.0f }; // useTexture = 0.0
                m_shaderManager->UpdatePerMaterial(m_graphics->GetContext(), sphereMat);

                m_sphereMesh->Draw(m_graphics->GetContext());
                };

            // パス1：シャドウマップ生成
            m_shadowMap->Bind(m_graphics->GetContext());
            //プレイヤーの画像を影のパスにも渡す
            m_shaderManager->BindShadowPass(m_graphics->GetContext(), m_playerTexture->GetSRV());
            m_shaderManager->UpdatePerFrame(m_graphics->GetContext(), frameData);
            DrawScene(true);

            // パス2：メイン描画
            m_graphics->SetMainRenderTarget();
            m_graphics->Clear(currentSkyColor.x, currentSkyColor.y, currentSkyColor.z, 1.0f);
            m_shaderManager->BindMainPass(m_graphics->GetContext(), m_shadowMap->GetSRV());

            // 地上の物体を描画
            DrawScene(false);

            // 太陽

            CBPerObject sunObj;
            sunObj.worldMatrix = XMMatrixTranspose(XMMatrixTranslationFromVector(sunPos));
            m_shaderManager->UpdatePerObject(m_graphics->GetContext(), sunObj);
            CBPerMaterial sunMat = { XMFLOAT4(1.0f, 1.0f, 0.8f, 1.0f), 0.0f, 0.0f, 10.0f, 0.0f };
            m_shaderManager->UpdatePerMaterial(m_graphics->GetContext(), sunMat);
            m_sphereMesh->Draw(m_graphics->GetContext());

            // 月
            CBPerObject moonObj;
            moonObj.worldMatrix = XMMatrixTranspose(XMMatrixScaling(0.6f, 0.6f, 0.6f) * XMMatrixTranslationFromVector(moonPos));
            m_shaderManager->UpdatePerObject(m_graphics->GetContext(), moonObj);
            CBPerMaterial moonMat = { XMFLOAT4(0.8f, 0.8f, 1.0f, 1.0f), 0.0f, 0.0f, 3.0f, 0.0f };
            m_shaderManager->UpdatePerMaterial(m_graphics->GetContext(), moonMat);
            m_sphereMesh->Draw(m_graphics->GetContext());

            // 後処理
            ID3D11ShaderResourceView* nullSRV = nullptr;
            m_graphics->GetContext()->PSSetShaderResources(0, 1, &nullSRV);
            m_graphics->Present();
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