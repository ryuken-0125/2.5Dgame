#pragma once
/*-----------------------------------------------------------------------------
 * @file  game_context.h
 * @brief シーン間で共有する DirectX リソースおよびシステムポインタの集約構造体
 *-----------------------------------------------------------------------------
 * [使い方]
 * 1. Application クラスで各リソースの実体を生成する。
 * 2. この構造体にポインタ（アドレス）をセットする。
 * 3. 各シーンの Update / Draw に参照渡しで引き回す。
 *----------------------------------------------------------------------------*/

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#include "Graphics.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "ShadowMap.h"
#include "Texture.h"

struct GameContext
{

    // --- 【追加】DirectXの直接ポインタ ---
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    // --- システム系ポインタ ---
    // 描画基盤（デバイス取得、レンダーターゲット切り替え用）
    Graphics* graphics = nullptr;
    // シェーダー管理（定数バッファ更新、ボーン制御、描画パス設定用）
    ShaderManager* shaderManager = nullptr;

    // --- 共有メッシュ（リソースの重複生成を避けるためここで一括管理） ---
    // 2Dスプライト・板ポリゴン用
    Mesh* quadMesh = nullptr;
    // 汎用ブロック・ワープゾーンの可視化用
    Mesh* cubeMesh = nullptr;
    // 地面用（スケールを変更して利用することを想定）
    Mesh* floorMesh = nullptr;
    // 球体（ライトのデバッグ表示や装飾用）
    Mesh* sphereMesh = nullptr;

    // --- 特殊リソース ---
    // シャドウマップ（影の描画パスで使用）
    ShadowMap* shadowMap = nullptr;

    // --- 共通テクスチャ ---
    // プレイヤーのキャラチップ/画像
    Texture* playerTexture = nullptr;
    // 地形・マップチップ用のテクスチャ
    Texture* tilesetTexture = nullptr;

    // [Tips] ここに ID3D11Device* 等を直接持たせておくと、
    // graphics->GetDevice() と書かずに済むのでコードがスッキリします
};