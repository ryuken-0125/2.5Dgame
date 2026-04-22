#include "scene_manager.h"

//--------------------------------------
// シーン生成（SceneManagerを渡す）
//--------------------------------------
std::unique_ptr<Scene> CreateScene(SceneType type, SceneManager& sm)
{
    switch (type)
    {
    //case SceneType::TITLE:  return std::make_unique<Title>(sm);
    //case SceneType::GAME:   return std::make_unique<Game>(sm);
    //case SceneType::RESULT: return std::make_unique<Result>(sm);
    }
    return nullptr;
}

//--------------------------------------
// シーン切り替え要求
//--------------------------------------
void SceneManager::ChangeScene(SceneType type)
{
    nextSceneType = type;
    isChanging = true;
}

//--------------------------------------
// 更新処理
//--------------------------------------
void SceneManager::Update(double deltaTime)
{
    // 安全なタイミングで切り替え
    if (isChanging)
    {
        if (currentScene)
        {
            currentScene->Finalize();
        }

        currentScene = CreateScene(nextSceneType, *this);

        if (currentScene)
        {
            currentScene->Initialize();
        }

        currentType = nextSceneType;
        isChanging = false;
    }

    if (currentScene)
    {
        currentScene->Update(deltaTime);
    }
}

//--------------------------------------
// 描画処理
//--------------------------------------
void SceneManager::Draw()
{
    if (currentScene)
    {
        currentScene->Draw();
    }
}

//使い方

//#include "title.h"
//#include "scene_manager.h"
//
//Title::Title(SceneManager& sm)
//    : Scene(sm)
//{
//}
//
//void Title::Update(double dt)
//{
//    if (/* スタート押された */)
//    {
//        sceneManager.ChangeScene(SceneType::GAME);
//    }
//}

//Cのグローバル状態じゃないから、クラスのインスタンスを...