/*------------------------------
 * field_scene.cpp - Field / map-select scene
 *------------------------------*/

#include "field_scene.h"
#include "scene_manager.h"
#include <DirectXMath.h>

using namespace DirectX;

FieldScene::FieldScene(SceneManager& sm, GameContext& ctx)
    : Scene(sm)
    , m_ctx(ctx)
    , m_playerPos(0.0f, 0.0f, 0.0f)
    , m_isWideCamera(false)
    , m_angle(0.0f)
{
}

void FieldScene::Initialize()
{
    m_playerPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_isWideCamera = false;
    m_angle = 0.0f;

    // --- ステータス初期化 ---
    m_playerStatus.Initialize();

    m_camera.SetPosition(0.0f, 2.0f, -8.0f);
    m_camera.SetFOV(XMConvertToRadians(90.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    SetupWarpZones();
    m_shopUI.SetShopManager(&m_shopManager);

    m_effect.Initialize(m_ctx.graphics->GetDevice(), "../asset/texture/effect1.png");
}

void FieldScene::Finalize() {}  

void FieldScene::SetupWarpZones()
{
    m_warpZones.AddZone(
        AABB::Make(XMFLOAT3(-5.0f, 0.5f, 5.0f), XMFLOAT3(3.0f, 2.5f, 3.0f)),
        0, "Scene 0", XMFLOAT4(0.2f, 0.9f, 0.2f, 1.0f));

    m_warpZones.AddZone(
        AABB::Make(XMFLOAT3(5.0f, 0.5f, 5.0f), XMFLOAT3(3.0f, 2.5f, 3.0f)),
        1, "Scene 1", XMFLOAT4(0.2f, 0.4f, 1.0f, 1.0f));

    m_warpZones.AddZone(
        AABB::Make(XMFLOAT3(-5.0f, 0.5f, -5.0f), XMFLOAT3(3.0f, 2.5f, 3.0f)),
        2, "Scene 2", XMFLOAT4(1.0f, 0.6f, 0.1f, 1.0f));

    m_warpZones.AddZone(
        AABB::Make(XMFLOAT3(5.0f, 0.5f, -5.0f), XMFLOAT3(3.0f, 2.5f, 3.0f)),
        3, "Scene 3", XMFLOAT4(0.9f, 0.2f, 0.9f, 1.0f));
}

void FieldScene::Update(double deltaTime)
{
    float dt = (float)deltaTime;

    // ================================================
    // P キーでショップ開閉（押した瞬間だけ）
    // ================================================
    bool pNow = (GetAsyncKeyState('P') & 0x8000) != 0;
    if (pNow && !m_pWasDown)
    {
        if (m_shopUI.IsOpen()) m_shopUI.Close();
        else                   m_shopUI.Open();
    }
    m_pWasDown = pNow;

    m_shopUI.Update(static_cast<float>(deltaTime));

    // ↓ これを追加
    if (m_shopUI.WantsToClose())
        m_shopUI.Close();

    // ショップが開いている間はフィールドの更新をスキップ
    if (m_shopUI.IsOpen()) return;

    // ================================================
    // ショップが開いている間はゲームを止めてUIへ委譲
    // ================================================
    if (m_shopUI.IsOpen())
    {
        m_shopUI.Update(dt);

        // カメラ・ライトだけ更新してフィールドを止める
        XMFLOAT3 offset(0.0f, 12.0f, -18.0f);
        m_camera.SetFollowTarget(m_playerPos, offset);
        m_camera.Update();
        if (m_ctx.lightManager)
        {
            m_ctx.lightManager->Update(dt);
            m_ctx.lightManager->UpdateShadowCamera(m_playerPos);
        }
        return;  // 移動・ワープゾーン処理をスキップ
    }

    // ================================================
    // 以下は既存コード（ショップが閉じている時のみ）
    // ================================================
    if (m_move.CheckStunKey()) m_playerStatus.ApplyStun();
    if (m_move.CheckSlowKey()) m_playerStatus.ApplySlow();

    m_move.ControlPlayer(m_playerPos, dt, m_playerStatus);
    m_playerStatus.Update(dt, false);

    if (m_move.IsActionTriggered())
    {
        // 定数：プレイヤーからどれくらい離れた位置に出すか
        const float EFFECT_DISTANCE = 2.0f;

        // プレイヤーの現在位置をベースにする
        DirectX::XMFLOAT3 effectPosition = m_playerPos;

        // Moveクラスから向いている方向ベクトルを取得
        DirectX::XMFLOAT3 facingDirection = m_move.GetFacingDirection();

        // 座標を「向いている方向へ、DISTANCE分だけずらす」
        effectPosition.x += facingDirection.x * EFFECT_DISTANCE;
        effectPosition.z += facingDirection.z * EFFECT_DISTANCE;

        // ずらした座標でエフェクトを再生
        m_effect.Play(effectPosition);
    }
    m_effect.Update(dt);

    /*
    if (GetAsyncKeyState('J') & 0x8000)
    {
        m_effect.Play(m_playerPos);
    }
    m_effect.Update(dt); // アニメーションのコマ送り
    */

    if (m_move.CheckFovToggle())
    {
        m_isWideCamera = !m_isWideCamera;
        float fov = m_isWideCamera ? XMConvertToRadians(120.0f)
            : XMConvertToRadians(90.0f);
        m_camera.SetFOV(fov, 1280.0f / 720.0f, 0.1f, 100.0f);
    }

    XMFLOAT3 offset(0.0f, 12.0f, -18.0f);
    m_camera.SetFollowTarget(m_playerPos, offset);
    m_camera.Update();

    const float DAY_DURATION = 10.0f;
    m_angle += (float)(2.0 * 3.14159265 / DAY_DURATION * deltaTime);

    if (m_ctx.lightManager)
    {
        m_ctx.lightManager->Update(dt);
        m_ctx.lightManager->UpdateShadowCamera(m_playerPos);
    }

    int warpResult = m_warpZones.Update(m_playerPos);
    if (warpResult >= 0)
        m_sceneManager.ChangeToSubScene(warpResult);
}

void FieldScene::Draw()
{
    auto* ctx = m_ctx.graphics->GetContext();
    auto* sm = m_ctx.shaderManager;

    // Moon direction (fixed)
    XMVECTOR moonDir = XMVector3Normalize(XMVectorSet(-0.3f, -1.0f, 0.5f, 0.0f));
    XMVECTOR moonPos = XMVectorSet(0.0f, 20.0f, 0.0f, 1.0f) - moonDir * 25.0f;

    float    moonIntensity = max(0.0f, sinf(m_angle + XM_PI));
    XMFLOAT3 moonColor(0.7f * moonIntensity, 0.8f * moonIntensity, 1.2f * moonIntensity);

    // Build per-frame constant buffer
    CBPerFrame frameData = {};
    if (m_ctx.lightManager)
    {
        frameData = m_ctx.lightManager->GetFrameData(
            m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix(),
            m_camera.GetPosition()
        );
    }
    else
    {
        frameData.viewProjection = XMMatrixTranspose(
            m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix()
        );
        frameData.cameraPos = m_camera.GetPosition();
    }

    // Directional light (shared)
    XMVECTOR lightDirVec = XMVector3Normalize(XMVectorSet(0.3f, -1.0f, 0.5f, 0.0f));
    XMStoreFloat3(&frameData.lightDir, lightDirVec);
    frameData.lightColor = XMFLOAT3(1.0f, 1.0f, 0.9f);

    // Sun (rotates with day-night cycle)
    XMVECTOR sunDirVec = XMVector3Normalize(
        XMVectorSet(sinf(m_angle), -cosf(m_angle), 0.3f, 0.0f));
    XMStoreFloat3(&frameData.sunDir, sunDirVec);
    float sunIntensity = max(0.0f, -XMVectorGetY(sunDirVec));
    frameData.sunColor = XMFLOAT3(1.0f * sunIntensity, 0.9f * sunIntensity,
        0.7f * sunIntensity);

    // Moon
    XMStoreFloat3(&frameData.moonDir, moonDir);
    frameData.moonColor = moonColor;
    frameData.skyColor = XMFLOAT4(0.1f, 0.1f, 0.4f, 1.0f);

    // Light view-projection for shadow pass
    XMVECTOR playerVec = XMLoadFloat3(&m_playerPos);
    XMVECTOR lightPos = playerVec + lightDirVec * -30.0f;
    XMMATRIX lightView = XMMatrixLookAtLH(lightPos, playerVec,
        XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
    XMMATRIX lightProj = XMMatrixOrthographicLH(40.0f, 40.0f, 0.1f, 100.0f);
    frameData.lightViewProjection = XMMatrixTranspose(lightView * lightProj);

    // --- Shadow pass ---
    m_ctx.shadowMap->Bind(ctx);
    sm->BindShadowPass(ctx, m_ctx.playerTexture->GetSRV());
    sm->UpdatePerFrame(ctx, frameData);
    DrawScene(true);

    // --- Main pass ---
    m_ctx.graphics->SetMainRenderTarget();
    m_ctx.graphics->Clear(0.1f, 0.1f, 0.4f, 1.0f);
    sm->BindMainPass(ctx, m_ctx.shadowMap->GetSRV());
    sm->UpdatePerFrame(ctx, frameData);
    DrawScene(false);

    // Draw moon sphere
    CBPerObject moonObj;
    moonObj.worldMatrix = XMMatrixTranspose(
        XMMatrixScaling(0.6f, 0.6f, 0.6f) *
        XMMatrixTranslationFromVector(moonPos)
    );
    sm->UpdatePerObject(ctx, moonObj);
    CBPerMaterial moonMat = { XMFLOAT4(0.8f, 0.8f, 1.0f, 1.0f), 0.0f, 0.0f, 3.0f, 0.0f };
    sm->UpdatePerMaterial(ctx, moonMat);
    m_ctx.sphereMesh->Draw(ctx);

    // Draw warp zones（既存）
    m_warpZones.Draw(ctx, sm, m_ctx.cubeMesh);

    // ================================================
    // ショップUI（最後に2Dオーバーレイとして描画）
    // ※ ClearDepthOnly() でシーンのデプスをリセットし
    //   UIが必ず手前に表示されるようにする
    // ================================================
    if (m_shopUI.IsOpen())
    {
        m_ctx.graphics->ClearDepthOnly();
        m_shopUI.Draw(ctx, sm, m_ctx.cubeMesh, 1280, 720);
    }

    // Cleanup（既存）
    ID3D11ShaderResourceView* nullSRV = nullptr;
    ctx->PSSetShaderResources(0, 1, &nullSRV);
}

void FieldScene::DrawScene(bool isShadowPass)
{
    auto* ctx = m_ctx.graphics->GetContext();
    auto* sm = m_ctx.shaderManager;

    // 1. Floor
    CBPerObject floorObj;
    floorObj.worldMatrix = XMMatrixTranspose(
        XMMatrixScaling(40.0f, 0.1f, 40.0f) *
        XMMatrixTranslation(0.0f, -0.1f, 0.0f)
    );
    floorObj.uvOffset = XMFLOAT2(0.0f, 0.0f); 
    floorObj.uvScale = XMFLOAT2(1.0f, 1.0f);  
    sm->UpdatePerObject(ctx, floorObj);
    CBPerMaterial floorMat = { XMFLOAT4(0.3f, 0.5f, 0.3f, 1.0f), 0.8f, 0.0f, 0.0f, 0.0f };
    sm->UpdatePerMaterial(ctx, floorMat);
    m_ctx.floorMesh->Draw(ctx);

    // 2. Player (2D sprite quad)
    CBPerObject playerObj;
    XMMATRIX scale = XMMatrixScaling(1.5f, 1.5f, 1.5f);
    XMMATRIX rot = XMMatrixRotationX(XMConvertToRadians(30.0f));
    XMMATRIX trans = XMMatrixTranslation(m_playerPos.x, m_playerPos.y, m_playerPos.z);
    playerObj.worldMatrix = XMMatrixTranspose(scale * rot * trans);
    playerObj.uvOffset = XMFLOAT2(0.0f, 0.0f); 
    playerObj.uvScale = XMFLOAT2(1.0f, 1.0f);  
    sm->UpdatePerObject(ctx, playerObj);

    CBPerMaterial playerMat = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.9f, 0.0f, 0.0f, 1.0f };
    sm->UpdatePerMaterial(ctx, playerMat);

    if (!isShadowPass)
    {
        ID3D11ShaderResourceView* pSRV = m_ctx.playerTexture->GetSRV();
        ctx->PSSetShaderResources(1, 1, &pSRV);
    }
    m_ctx.quadMesh->Draw(ctx);

    if (!isShadowPass)
    {
        ID3D11ShaderResourceView* nullSRV = nullptr;
        ctx->PSSetShaderResources(1, 1, &nullSRV);
    }

    // 3. Center sphere
    CBPerObject sphereObj;
    sphereObj.worldMatrix = XMMatrixTranspose(XMMatrixTranslation(0.0f, 1.0f, 0.0f));
    sphereObj.uvOffset = XMFLOAT2(0.0f, 0.0f); 
    sphereObj.uvScale = XMFLOAT2(1.0f, 1.0f);  
    sm->UpdatePerObject(ctx, sphereObj);
    CBPerMaterial sphereMat = { XMFLOAT4(0.56f, 0.57f, 0.58f, 1.0f), 0.1f, 1.0f, 0.0f, 0.0f };
    sm->UpdatePerMaterial(ctx, sphereMat);
    m_ctx.sphereMesh->Draw(ctx);

    if (!isShadowPass)
    {
        m_effect.Draw(ctx, sm, m_ctx.quadMesh);
    }

}
