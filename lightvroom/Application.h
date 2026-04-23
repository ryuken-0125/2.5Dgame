
#pragma once
#include <Windows.h>
#include <memory>
#include "Graphics.h"
#include "ShaderManager.h"
#include "ShadowMap.h"
#include "Mesh.h"
#include "Texture.h"
#include "game_context.h"
#include "scene_manager.h"
#include "Camera.h"
#include "Move.h"

class Application
{
public:
    Application();
    ~Application();

    bool Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height);
    void Run();
    void Release(); 

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  
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

    // --- Update
    float m_elapsedTime;
    DirectX::XMFLOAT4 m_currentSkyColor;
    DirectX::XMFLOAT3 m_moonPos;

    DirectX::XMFLOAT3 m_eyePos;          
    DirectX::XMFLOAT3 m_lightTargetPos;   
    DirectX::XMFLOAT3 m_lightMoveGoal;    
    float m_lightMoveTimer;               

    CBPerFrame m_frameData;

std::unique_ptr<Texture> m_tilesetTexture;

GameContext m_gameContext;
SceneManager m_sceneManager;
};

