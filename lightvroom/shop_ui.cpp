/*------------------------------
 * shop_ui.cpp
 *------------------------------*/
#define NOMINMAX
#include "shop_ui.h"
#include <Windows.h>
#include <algorithm>

using namespace DirectX;

/*====================================================================
 * Update: 入力処理 + 演出タイマー更新
 *====================================================================*/
void ShopUI::Update(float deltaTime)
{
    if (!m_isOpen || !m_shopManager) return;

    // ---- Esc で閉じる ----
    m_wantsToClose = false;                          // 毎フレームリセット
    bool escNow = (GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0;
    if (escNow && !m_escWasDown)
    {
        m_wantsToClose = true;
        m_escWasDown = escNow;
        return;                                      // 他の入力は無視
    }
    m_escWasDown = escNow;

    const int itemCount = static_cast<int>(m_shopManager->GetItems().size());

    // 「押した瞬間だけ true」を返すラムダ
    auto justPressed = [](int vk, bool& wasDown) -> bool {
        bool now = (GetAsyncKeyState(vk) & 0x8000) != 0;
        bool result = now && !wasDown;
        wasDown = now;
        return result;
        };

    // --- 方向キーでカーソル移動 ---
    if (justPressed(VK_LEFT, m_leftWasDown))
        m_selectedIndex = (m_selectedIndex - 1 + itemCount) % itemCount;

    if (justPressed(VK_RIGHT, m_rightWasDown))
        m_selectedIndex = (m_selectedIndex + 1) % itemCount;

    // 上下はグリッドの列数(COLS)分移動
    if (justPressed(VK_UP, m_upWasDown))
        m_selectedIndex = (m_selectedIndex - COLS + itemCount) % itemCount;

    if (justPressed(VK_DOWN, m_downWasDown))
        m_selectedIndex = (m_selectedIndex + COLS) % itemCount;

    // --- Enter キーで購入 ---
    bool enterNow = (GetAsyncKeyState(VK_RETURN) & 0x8000) != 0;
    if (enterNow && !m_enterWasDown)
    {
        m_flashSuccess = m_shopManager->TryPurchase(m_selectedIndex);
        m_flashTimer = 0.25f;   // 0.25秒フラッシュ
    }
    m_enterWasDown = enterNow;

    // --- フラッシュタイマー更新 ---
    if (m_flashTimer > 0.0f)
    {
        m_flashTimer -= deltaTime;
        if (m_flashTimer < 0.0f) m_flashTimer = 0.0f;
    }
}

/*====================================================================
 * Draw: UI全体の描画
 *
 * 【描画の仕組み】
 *   既存の PBR シェーダーを流用し、オルソ射影に切り替えて
 *   画面ピクセル座標で矩形を並べる。
 *   ライティングをゼロにして emissive 値で発色させる。
 *   depth 値を奥 (0.9) → 手前 (0.3) の順で使うことで
 *   正しい重なり順を実現している。
 *====================================================================*/
void ShopUI::Draw(ID3D11DeviceContext* ctx, ShaderManager* sm,
    Mesh* cubeMesh, int screenW, int screenH)
{
    if (!m_isOpen || !m_shopManager || !cubeMesh) return;

    // -------------------------------------------------------
    // 2D描画専用のオルソ射影を設定
    // (0,0) = 画面左上, (screenW, screenH) = 画面右下
    // -------------------------------------------------------
    XMMATRIX ortho = XMMatrixOrthographicOffCenterLH(
        0.0f, (float)screenW,
        (float)screenH, 0.0f,   // bottom > top で Y軸を下方向に
        0.0f, 1.0f
    );

    // ライティングをゼロにした CBPerFrame で ortho 射影を適用
    CBPerFrame uiFrame = {};
    uiFrame.viewProjection = XMMatrixTranspose(ortho);
    sm->UpdatePerFrame(ctx, uiFrame);

    const auto& items = m_shopManager->GetItems();

    // =======================================================
    // 1. 背景パネル（最奥: depth 0.90）
    // =======================================================
    DrawRect(ctx, sm, cubeMesh,
        140.0f, 20.0f, 1000.0f, 680.0f,
        XMFLOAT4(0.08f, 0.06f, 0.05f, 1.0f), 1.2f, 0.90f);

    // =======================================================
    // 2. タイトルバー「ショップ」
    // =======================================================
    DrawRect(ctx, sm, cubeMesh,
        150.0f, 30.0f, 600.0f, 60.0f,
        XMFLOAT4(0.55f, 0.35f, 0.10f, 1.0f), 2.5f, 0.80f);

    // =======================================================
    // 3. コイン表示バー（右上）
    // =======================================================
    // バー背景
    DrawRect(ctx, sm, cubeMesh,
        870.0f, 30.0f, 260.0f, 55.0f,
        XMFLOAT4(0.80f, 0.65f, 0.05f, 1.0f), 2.5f, 0.80f);

    // コイン残量ゲージ（最大 200 コインとして比例表示）
    float coinRatio = std::min(m_shopManager->GetCoin() / 200.0f, 1.0f);
    DrawRect(ctx, sm, cubeMesh,
        878.0f, 44.0f, 244.0f * coinRatio, 28.0f,
        XMFLOAT4(1.0f, 0.90f, 0.10f, 1.0f), 3.5f, 0.75f);

    // =======================================================
    // 4. アイテムグリッド（4列 × 2行）
    // =======================================================
    constexpr float SLOT_W = 230.0f;
    constexpr float SLOT_H = 265.0f;
    constexpr float GAP = 10.0f;
    constexpr float GRID_X = 155.0f;
    constexpr float GRID_Y = 110.0f;

    for (int i = 0; i < static_cast<int>(items.size()) && i < COLS * ROWS; ++i)
    {
        int   col = i % COLS;
        int   row = i / COLS;
        float sx = GRID_X + col * (SLOT_W + GAP);
        float sy = GRID_Y + row * (SLOT_H + GAP);

        const ShopItem& item = items[i];
        bool            selected = (i == m_selectedIndex);
        bool            soldOut = (item.stock == 0);

        // --- スロット枠（選択中はオレンジに光る）---
        XMFLOAT4 borderColor = selected
            ? XMFLOAT4(1.0f, 0.75f, 0.0f, 1.0f)
            : XMFLOAT4(0.40f, 0.30f, 0.15f, 1.0f);

        DrawRect(ctx, sm, cubeMesh,
            sx, sy, SLOT_W, SLOT_H,
            borderColor, selected ? 5.0f : 1.8f, 0.70f);

        // --- スロット内側（アイテムカラー）---
        XMFLOAT4 innerColor = soldOut
            ? XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f)
            : item.color;

        DrawRect(ctx, sm, cubeMesh,
            sx + 6.0f, sy + 6.0f, SLOT_W - 12.0f, SLOT_H - 62.0f,
            innerColor, selected ? 2.8f : 1.8f, 0.60f);

        // --- 価格バー（スロット下部）---
        bool canAfford = (m_shopManager->GetCoin() >= item.price && !soldOut);

        // バー背景（購入可能=暗茶 / 不可=暗赤）
        DrawRect(ctx, sm, cubeMesh,
            sx + 6.0f, sy + SLOT_H - 55.0f, SLOT_W - 12.0f, 49.0f,
            canAfford
            ? XMFLOAT4(0.15f, 0.12f, 0.05f, 1.0f)
            : XMFLOAT4(0.25f, 0.05f, 0.05f, 1.0f),
            1.5f, 0.55f);

        // コスト充足ゲージ（持ちコイン ÷ 価格、最大 1.0）
        float gaugeRatio = std::min((float)m_shopManager->GetCoin() / (float)item.price, 1.0f);
        DrawRect(ctx, sm, cubeMesh,
            sx + 10.0f, sy + SLOT_H - 48.0f, (SLOT_W - 20.0f) * gaugeRatio, 20.0f,
            canAfford
            ? XMFLOAT4(0.20f, 0.80f, 0.20f, 1.0f)
            : XMFLOAT4(0.70f, 0.10f, 0.10f, 1.0f),
            2.5f, 0.50f);

        // --- "NEW" バッジ（緑の小矩形）---
        if (item.isNew)
        {
            DrawRect(ctx, sm, cubeMesh,
                sx + 8.0f, sy + 8.0f, 55.0f, 25.0f,
                XMFLOAT4(0.0f, 0.90f, 0.10f, 1.0f), 4.0f, 0.45f);
        }

        // --- 有限在庫インジケーター（最大 10 個として比例表示）---
        if (item.stock > 0)
        {
            float stockRatio = std::min(item.stock / 10.0f, 1.0f);
            DrawRect(ctx, sm, cubeMesh,
                sx + 8.0f, sy + 35.0f, (SLOT_W - 16.0f) * stockRatio, 12.0f,
                XMFLOAT4(0.90f, 0.90f, 0.20f, 1.0f), 2.0f, 0.45f);
        }

        // --- 在庫切れオーバーレイ（暗く塗りつぶす）---
        if (soldOut)
        {
            DrawRect(ctx, sm, cubeMesh,
                sx + 6.0f, sy + 6.0f, SLOT_W - 12.0f, SLOT_H - 62.0f,
                XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), 0.5f, 0.42f);
        }

        // --- 購入フラッシュ演出（選択中スロットのみ）---
        if (selected && m_flashTimer > 0.0f)
        {
            float intensity = m_flashTimer / 0.25f;   // 経過とともにフェードアウト
            XMFLOAT4 flashColor = m_flashSuccess
                ? XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)    // 成功：白
                : XMFLOAT4(1.0f, 0.10f, 0.10f, 1.0f);  // 失敗：赤
            DrawRect(ctx, sm, cubeMesh,
                sx + 6.0f, sy + 6.0f, SLOT_W - 12.0f, SLOT_H - 62.0f,
                flashColor, 6.0f * intensity, 0.40f);
        }
    }

    // =======================================================
    // 5. 操作ガイドバー（最前面）
    // =======================================================
    DrawRect(ctx, sm, cubeMesh,
        150.0f, 650.0f, 980.0f, 40.0f,
        XMFLOAT4(0.30f, 0.25f, 0.10f, 1.0f), 2.0f, 0.35f);
}

/*====================================================================
 * DrawRect: 画面座標（ピクセル）で矩形を描画する共通処理
 * Cube Mesh（-1〜+1）を half-size スケーリングしてピクセル座標に配置する
 *====================================================================*/
void ShopUI::DrawRect(ID3D11DeviceContext* ctx, ShaderManager* sm, Mesh* cubeMesh,
    float x, float y, float w, float h,
    XMFLOAT4 color, float emissive, float depth)
{
    float cx = x + w * 0.5f;
    float cy = y + h * 0.5f;

    CBPerObject obj;
    obj.worldMatrix = XMMatrixTranspose(
        XMMatrixScaling(w * 0.5f, h * 0.5f, 0.01f) *
        XMMatrixTranslation(cx, cy, depth)
    );
    sm->UpdatePerObject(ctx, obj);

    CBPerMaterial mat;
    mat.albedo = color;
    mat.roughness = 1.0f;
    mat.metallic = 0.0f;
    mat.emissive = emissive;  // 高い値でライティングに依存せず発色
    mat.useTexture = 0.0f;
    sm->UpdatePerMaterial(ctx, mat);

    cubeMesh->Draw(ctx);
}