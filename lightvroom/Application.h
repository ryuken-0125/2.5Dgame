#pragma once
#include <Windows.h>
#include <memory>

#include "Graphics.h"
#include "ShaderManager.h" 
#include "Mesh.h"
#include "ShadowMap.h"
#include "Texture.h"
#include "PlayerManager.h" // PlayerManagerは必要
#include "game_context.h"
#include "scene_manager.h"

class Application
{
public:
    Application();
    ~Application();

    bool Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height);
    void Run();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd;
    HINSTANCE m_hInstance;

    // --- グラフィックス・リソース（共通） ---
    std::unique_ptr<Graphics> m_graphics;
    std::unique_ptr<ShaderManager> m_shaderManager; 

    // --- メッシュ・リソース（GameContextで共有するため保持） ---
    std::unique_ptr<Mesh> m_cubeMesh;
    std::unique_ptr<Mesh> m_sphereMesh;
    std::unique_ptr<Mesh> m_floorMesh;//�n�ʗp
    std::unique_ptr<Mesh> m_quadMesh;//2Dplayer�p

// --- 各種リソース ---
    std::unique_ptr<ShadowMap> m_shadowMap;//�V���h�E�}�b�v�p�N���X�̃C���X�^���X��ێ�����ϐ�
    std::unique_ptr<Texture> m_playerTexture;//2Dplayer�p�e�N�X�`��
    std::unique_ptr<Texture> m_tilesetTexture;

    // --- 設計の核（masterブランチの成果） ---
    // SceneManagerに全てのロジックを委譲する
    GameContext m_gameContext;
    SceneManager m_sceneManager;

    // ※ 注: PlayerManagerやCameraは、このApplicationクラスではなく、
    // SceneManagerが管理する「シーン（FieldSceneなど）」の中で
    // 保持・生成するように移植してください。

    Camera m_camera;
    //Move m_move;
    //DirectX::XMFLOAT3 m_playerPos; // �v���C���[�i�����́j�̌��݈ʒu
    std::unique_ptr<PlayerManager> m_playerManager;
};
