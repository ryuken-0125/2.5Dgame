
#pragma once
#include <windows.h>
#include <memory>
#include "Graphics.h"
#include "ShaderManager.h"
#include "ShadowMap.h"
#include "Mesh.h"
#include "Camera.h"
#include "Move.h"
#include "Texture.h"

class Application
{
public:
    Application();
    ~Application();

    bool Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height);
    void Run();
    void Release(); // ★追加：明示的な解放処理

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // ★追加：ループの中身を役割ごとに分割
    void Update(float deltaTime);
    void Draw();
    void DrawScene(bool isShadowPass);

    HWND m_hwnd;
    HINSTANCE m_hInstance;

    std::unique_ptr<Graphics> m_graphics;
    std::unique_ptr<ShaderManager> m_shaderManager;
    std::unique_ptr<ShadowMap> m_shadowMap;

    std::unique_ptr<Mesh> m_cubeMesh;
    std::unique_ptr<Mesh> m_sphereMesh;
    std::unique_ptr<Mesh> m_floorMesh;
    std::unique_ptr<Mesh> m_quadMesh;

    std::unique_ptr<Texture> m_playerTexture;

    Camera m_camera;
    Move m_move;
    DirectX::XMFLOAT3 m_playerPos;
    bool m_isWideCamera;

    // --- UpdateとDrawで共有するデータ ---
    float m_elapsedTime;
    DirectX::XMFLOAT4 m_currentSkyColor;
    DirectX::XMFLOAT3 m_moonPos;

    DirectX::XMFLOAT3 m_eyePos;          // 目の位置
    DirectX::XMFLOAT3 m_lightTargetPos;   // 現在の光の目標地点（地面上）
    DirectX::XMFLOAT3 m_lightMoveGoal;    // 次の移動先
    float m_lightMoveTimer;               // 移動タイマー

    CBPerFrame m_frameData;
};