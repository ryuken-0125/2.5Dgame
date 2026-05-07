/*------------------------------
 * shop_ui.h
 * ショップのUI描画と入力処理
 *------------------------------*/
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "ShaderManager.h"
#include "Mesh.h"
#include "shop_manager.h"

class ShopUI
{
public:
    ShopUI() = default;

    void SetShopManager(ShopManager* sm) { m_shopManager = sm; }

    bool IsOpen() const { return m_isOpen; }
    void Open() { m_isOpen = true;  m_selectedIndex = 0; }
    void Close() { m_isOpen = false; }

    // 毎フレーム呼ぶ（入力処理 + フラッシュタイマー更新）
    void Update(float deltaTime);
    bool WantsToClose() const { return m_wantsToClose; }

    // 毎フレーム呼ぶ（描画）
    // ※ 呼び出し前に Graphics::ClearDepthOnly() でデプスをリセットすること
    void Draw(ID3D11DeviceContext* ctx, ShaderManager* sm,
        Mesh* cubeMesh, int screenW, int screenH);

private:
    // 画面座標（ピクセル）で矩形を描画する共通処理
    void DrawRect(ID3D11DeviceContext* ctx, ShaderManager* sm, Mesh* cubeMesh,
        float x, float y, float w, float h,
        DirectX::XMFLOAT4 color, float emissive, float depth);

    bool  m_isOpen = false;
    int   m_selectedIndex = 0;

    // 購入フラッシュ演出
    float m_flashTimer = 0.0f;
    bool  m_flashSuccess = false;

    // キー入力の前フレーム状態（押した瞬間検出用）
    bool m_upWasDown = false;
    bool m_downWasDown = false;
    bool m_leftWasDown = false;
    bool m_rightWasDown = false;
    bool m_enterWasDown = false;

    ShopManager* m_shopManager = nullptr;

    static constexpr int COLS = 4;
    static constexpr int ROWS = 2;

    bool m_escWasDown = false;   // Esc キーの前フレーム状態
    bool m_wantsToClose = false;   // Esc が押されたフレームに true
};