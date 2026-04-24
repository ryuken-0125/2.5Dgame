/*------------------------------
 * sub_scene.cpp - Action sub-scene (2D side-scrolling tilemap)
 *------------------------------*/

#include "sub_scene.h"
#include "scene_manager.h"
#include <Windows.h>
#include <cmath>

using namespace DirectX;

// ============================================================
// Map data (0=empty,1=ground,2=platform,3=wall,4=exit,5=deco)
// Row 0 = top, Row MAP_HEIGHT-1 = bottom
// ============================================================

const int SubScene::MAP_DATA_0[TileMap::MAP_HEIGHT * TileMap::MAP_WIDTH] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,2,2,0,0,0,0,0,2,2,2,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,2,2,2,0,0,0,0,2,2,0,0,0,0,2,2,2,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,
    1,1,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,1,1,
    3,3,3,3,3,0,0,0,3,3,3,3,0,0,0,3,3,3,3,3,
    3,3,3,3,3,0,0,0,3,3,3,3,0,0,0,3,3,3,3,3,
};

const int SubScene::MAP_DATA_1[TileMap::MAP_HEIGHT * TileMap::MAP_WIDTH] =
{
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    3,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,3,
    3,0,0,2,2,0,0,0,0,0,0,0,0,0,0,2,2,0,0,3,
    3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    3,0,0,0,0,0,2,2,2,0,0,0,2,2,2,0,0,0,0,3,
    3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
    3,0,4,0,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,3,
    3,3,3,3,3,3,3,0,0,3,3,3,3,0,0,3,3,3,3,3,
    3,3,3,3,3,3,3,0,0,3,3,3,3,0,0,3,3,3,3,3,
};

const int SubScene::MAP_DATA_2[TileMap::MAP_HEIGHT * TileMap::MAP_WIDTH] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,
    0,0,2,0,0,0,0,2,2,0,0,0,0,0,0,0,2,2,0,0,
    0,0,0,0,0,2,0,0,0,0,0,0,0,2,2,0,0,0,0,0,
    0,4,0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,
    2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,
    0,0,0,2,2,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,2,2,2,0,0,0,0,0,2,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,
};

const int SubScene::MAP_DATA_3[TileMap::MAP_HEIGHT * TileMap::MAP_WIDTH] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0,0,0,
    0,0,2,2,0,0,3,0,0,2,2,0,0,3,0,0,2,2,0,0,
    0,0,0,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0,0,0,
    5,5,0,0,5,5,3,5,5,0,0,5,5,3,5,5,0,0,5,4,
    1,1,1,1,1,1,3,1,1,1,1,1,1,3,1,1,1,1,1,1,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
};

// ============================================================
// Constructor
// ============================================================
SubScene::SubScene(SceneManager& sm, GameContext& ctx, int index)
    : Scene(sm)
    , m_ctx(ctx)
    , m_index(index)
    , m_playerPos(0.0f, 0.0f, 0.0f)
    , m_playerVel(0.0f, 0.0f, 0.0f)
    , m_onGround(false)
    , m_jumpSpaceWasDown(false)
{
}

// ============================================================
// Initialize
// ============================================================
void SubScene::Initialize()
{
    LoadMap();
    SetupExitZone();

    m_playerPos = m_tileMap.GetSpawnPosition();
    m_playerVel = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_onGround  = false;
    m_jumpSpaceWasDown = false;

    m_camera.SetPosition(m_playerPos.x, m_playerPos.y + 5.0f, -18.0f);
    m_camera.SetFOV(XMConvertToRadians(60.0f), 1280.0f / 720.0f, 0.1f, 200.0f);
}

void SubScene::Finalize() {}

// ============================================================
// LoadMap
// ============================================================
void SubScene::LoadMap()
{
    const int* data = nullptr;
    switch (m_index)
    {
    case 0: data = MAP_DATA_0; break;
    case 1: data = MAP_DATA_1; break;
    case 2: data = MAP_DATA_2; break;
    case 3: data = MAP_DATA_3; break;
    default: data = MAP_DATA_0; break;
    }
    m_tileMap.LoadFromArray(data);
}

// ============================================================
// SetupExitZone
// ============================================================
void SubScene::SetupExitZone()
{
    XMFLOAT3 exitPos = m_tileMap.GetExitWorldPos();
    float ts = m_tileMap.tileSize;
    AABB exitBounds = AABB::Make(
        XMFLOAT3(exitPos.x, exitPos.y + ts * 0.5f, 0.0f),
        XMFLOAT3(ts * 1.5f, ts * 2.0f, ts * 3.0f)
    );
    m_exitZones.AddZone(exitBounds, -1, "Field", XMFLOAT4(0.9f, 0.9f, 0.1f, 0.7f));
}

// ============================================================
// Update
// ============================================================
void SubScene::Update(double deltaTime)
{
    float dt = (float)deltaTime;

    if (m_ctx.lightManager)
    {
        m_ctx.lightManager->Update(dt);
        m_ctx.lightManager->UpdateShadowCamera(m_playerPos);
    }

    HandleInput(dt);
    UpdatePhysics(dt);

    XMFLOAT3 camPos(
        m_playerPos.x,
        m_playerPos.y + 5.0f,
        m_tileMap.origin.z - 18.0f
    );
    m_camera.SetPosition(camPos.x, camPos.y, camPos.z);
    m_camera.Update();

    int exitResult = m_exitZones.Update(m_playerPos);
    if (exitResult == -1)
        m_sceneManager.ChangeScene(SceneType::FIELD);
}

// ============================================================
// HandleInput
// ============================================================
void SubScene::HandleInput(float dt)
{
    const float MOVE_SPEED = 8.0f;
    const float JUMP_SPEED = 15.0f;

    if (GetAsyncKeyState('A') & 0x8000)
        m_playerVel.x = -MOVE_SPEED;
    else if (GetAsyncKeyState('D') & 0x8000)
        m_playerVel.x = MOVE_SPEED;
    else
        m_playerVel.x = 0.0f;

    bool inExit = false;
    for (const auto& z : m_exitZones.GetZones())
    {
        Point p(m_playerPos);
        if (z.bounds.IsOverlap(p)) { inExit = true; break; }
    }

    bool spaceNow = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
    if (spaceNow && !m_jumpSpaceWasDown && m_onGround && !inExit)
    {
        m_playerVel.y = JUMP_SPEED;
        m_onGround = false;
    }
    m_jumpSpaceWasDown = spaceNow;
}

// ============================================================
// UpdatePhysics
// ============================================================
void SubScene::UpdatePhysics(float dt)
{
    const float GRAVITY = -28.0f;

    m_playerVel.y += GRAVITY * dt;
    m_playerPos.x += m_playerVel.x * dt;
    m_playerPos.y += m_playerVel.y * dt;

    float halfW = 0.6f;
    float halfH = 0.8f;

    m_onGround = false;
    float feetY = m_playerPos.y - halfH;
    float headY = m_playerPos.y + halfH;

    if (m_playerVel.y <= 0.0f)
    {
        if (m_tileMap.IsSolid(m_playerPos.x - halfW * 0.5f, feetY) ||
            m_tileMap.IsSolid(m_playerPos.x + halfW * 0.5f, feetY))
        {
            float localY = feetY - m_tileMap.origin.y;
            int row = TileMap::MAP_HEIGHT - 1 - (int)floorf(localY / m_tileMap.tileSize);
            if (row >= 0 && row < TileMap::MAP_HEIGHT)
            {
                float tileTopY = m_tileMap.origin.y +
                    (TileMap::MAP_HEIGHT - 1 - row + 1) * m_tileMap.tileSize;
                m_playerPos.y = tileTopY + halfH;
            }
            m_playerVel.y = 0.0f;
            m_onGround = true;
        }
    }
    else
    {
        if (m_tileMap.IsSolid(m_playerPos.x - halfW * 0.5f, headY) ||
            m_tileMap.IsSolid(m_playerPos.x + halfW * 0.5f, headY))
        {
            m_playerVel.y = -1.0f;
        }
    }

    float leftX  = m_playerPos.x - halfW;
    float rightX = m_playerPos.x + halfW;
    float midY   = m_playerPos.y;

    if (m_playerVel.x < 0.0f && m_tileMap.IsSolid(leftX, midY))
    {
        float localX   = leftX - m_tileMap.origin.x;
        int col        = (int)floorf(localX / m_tileMap.tileSize);
        float tileRightX = m_tileMap.origin.x + (col + 1) * m_tileMap.tileSize;
        m_playerPos.x  = tileRightX + halfW;
        m_playerVel.x  = 0.0f;
    }
    if (m_playerVel.x > 0.0f && m_tileMap.IsSolid(rightX, midY))
    {
        float localX   = rightX - m_tileMap.origin.x;
        int col        = (int)floorf(localX / m_tileMap.tileSize);
        float tileLeftX = m_tileMap.origin.x + col * m_tileMap.tileSize;
        m_playerPos.x  = tileLeftX - halfW;
        m_playerVel.x  = 0.0f;
    }

    float mapBottomY = m_tileMap.origin.y - m_tileMap.tileSize * 2.0f;
    if (m_playerPos.y < mapBottomY)
    {
        m_playerPos = m_tileMap.GetSpawnPosition();
        m_playerVel = XMFLOAT3(0.0f, 0.0f, 0.0f);
    }
}

// ============================================================
// Draw
// ============================================================
void SubScene::Draw()
{
    auto* ctx = m_ctx.graphics->GetContext();
    auto* sm  = m_ctx.shaderManager;

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

    XMVECTOR lightDir = XMVector3Normalize(XMVectorSet(0.3f, -1.0f, 0.5f, 0.0f));
    XMStoreFloat3(&frameData.lightDir,  lightDir);
    frameData.lightColor = XMFLOAT3(1.0f, 1.0f, 0.9f);
    XMStoreFloat3(&frameData.sunDir,   lightDir);
    frameData.sunColor   = XMFLOAT3(1.0f, 0.9f, 0.7f);
    XMStoreFloat3(&frameData.moonDir,
        XMVector3Normalize(XMVectorSet(-0.3f, -1.0f, 0.5f, 0.0f)));
    frameData.moonColor  = XMFLOAT3(0.4f, 0.4f, 0.6f);

    // Sky color per scene
    switch (m_index)
    {
    case 0: frameData.skyColor = XMFLOAT4(0.30f, 0.55f, 0.90f, 1.0f); break;
    case 1: frameData.skyColor = XMFLOAT4(0.03f, 0.03f, 0.05f, 1.0f); break;
    case 2: frameData.skyColor = XMFLOAT4(0.55f, 0.75f, 1.00f, 1.0f); break;
    case 3: frameData.skyColor = XMFLOAT4(0.15f, 0.10f, 0.05f, 1.0f); break;
    default:frameData.skyColor = XMFLOAT4(0.30f, 0.55f, 0.90f, 1.0f); break;
    }

    XMVECTOR playerVec = XMLoadFloat3(&m_playerPos);
    XMVECTOR lightPos  = playerVec + lightDir * -30.0f;
    XMMATRIX lightView = XMMatrixLookAtLH(lightPos, playerVec,
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    XMMATRIX lightProj = XMMatrixOrthographicLH(60.0f, 60.0f, 0.1f, 200.0f);
    frameData.lightViewProjection = XMMatrixTranspose(lightView * lightProj);

    // --- Shadow pass ---
    m_ctx.shadowMap->Bind(ctx);
    sm->BindShadowPass(ctx, m_ctx.playerTexture->GetSRV());
    sm->UpdatePerFrame(ctx, frameData);
    DrawScene();

    // --- Main pass ---
    m_ctx.graphics->SetMainRenderTarget();
    m_ctx.graphics->Clear(
        frameData.skyColor.x, frameData.skyColor.y,
        frameData.skyColor.z, 1.0f
    );
    sm->BindMainPass(ctx, m_ctx.shadowMap->GetSRV());
    sm->UpdatePerFrame(ctx, frameData);
    DrawScene();

    // Draw exit zone indicator
    m_exitZones.Draw(ctx, sm, m_ctx.cubeMesh);

    // Cleanup
    ID3D11ShaderResourceView* nullSRV = nullptr;
    ctx->PSSetShaderResources(0, 1, &nullSRV);
    m_ctx.graphics->Present();
}

// ============================================================
// DrawScene
// ============================================================
void SubScene::DrawScene()
{
    auto* ctx = m_ctx.graphics->GetContext();
    auto* sm  = m_ctx.shaderManager;

    // Tilemap
    m_tileMap.Draw(ctx, sm, m_ctx.cubeMesh);

    // Player sprite (2D quad, always faces camera)
    CBPerObject playerObj;
    XMMATRIX scale = XMMatrixScaling(1.5f, 1.5f, 1.5f);
    XMMATRIX trans = XMMatrixTranslation(m_playerPos.x, m_playerPos.y, 0.0f);
    playerObj.worldMatrix = XMMatrixTranspose(scale * trans);
    sm->UpdatePerObject(ctx, playerObj);

    CBPerMaterial playerMat = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.9f, 0.0f, 0.0f, 1.0f };
    sm->UpdatePerMaterial(ctx, playerMat);

    ID3D11ShaderResourceView* pSRV = m_ctx.playerTexture->GetSRV();
    ctx->PSSetShaderResources(1, 1, &pSRV);
    m_ctx.quadMesh->Draw(ctx);

    ID3D11ShaderResourceView* nullSRV = nullptr;
    ctx->PSSetShaderResources(1, 1, &nullSRV);
}