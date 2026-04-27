#include "scene_manager.h"
#include "game_context.h"
#include "field_scene.h"
#include "sub_scene.h"
#include "final_scene.h"

//--------------------------------------
// Init
//--------------------------------------
void SceneManager::Init(GameContext* ctx)
{
    m_ctx = ctx;
}

//--------------------------------------
// Scene factory
//--------------------------------------
static std::unique_ptr<Scene> CreateScene(SceneType type, SceneManager& sm, GameContext* ctx)
{
    switch (type)
    {
    case SceneType::FIELD:       return std::make_unique<FieldScene>(sm, *ctx);
    case SceneType::SUB_SCENE_0: return std::make_unique<SubScene>(sm, *ctx, 0);
    case SceneType::SUB_SCENE_1: return std::make_unique<SubScene>(sm, *ctx, 1);
    case SceneType::SUB_SCENE_2: return std::make_unique<SubScene>(sm, *ctx, 2);
    case SceneType::SUB_SCENE_3: return std::make_unique<SubScene>(sm, *ctx, 3);
    case SceneType::FINAL_STAGE: return std::make_unique<FinalScene>(sm, *ctx);
    default: return nullptr;
    }
}

//--------------------------------------
// ChangeScene
//--------------------------------------
void SceneManager::ChangeScene(SceneType type)
{
    m_nextType    = type;
    m_isChanging  = true;
}

//--------------------------------------
// ChangeToSubScene  (index 0-3)
//--------------------------------------
void SceneManager::ChangeToSubScene(int index)
{
    int clamped = (index < 0) ? 0 : (index > 3) ? 3 : index;
    SceneType types[] = {
        SceneType::SUB_SCENE_0,
        SceneType::SUB_SCENE_1,
        SceneType::SUB_SCENE_2,
        SceneType::SUB_SCENE_3
    };
    ChangeScene(types[clamped]);
}

void SceneManager::ChangeToFinalScene()
{
    ChangeScene(SceneType::FINAL_STAGE);
}

//--------------------------------------
// Update
//--------------------------------------
void SceneManager::Update(double deltaTime)
{
    if (m_isChanging)
    {
        if (m_currentScene)
            m_currentScene->Finalize();

        m_currentScene = CreateScene(m_nextType, *this, m_ctx);

        if (m_currentScene)
            m_currentScene->Initialize();

        m_currentType = m_nextType;
        m_isChanging  = false;
    }

    if (m_currentScene)
        m_currentScene->Update(deltaTime);
}

//--------------------------------------
// Draw
//--------------------------------------
void SceneManager::Draw()
{
    if (m_currentScene)
        m_currentScene->Draw();
}
