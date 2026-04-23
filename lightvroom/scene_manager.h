#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <memory>
#include "scene.h"

struct GameContext;

// Scene types
enum class SceneType
{
    FIELD,          // Field (map select)
    SUB_SCENE_0,    // Action scene 0
    SUB_SCENE_1,    // Action scene 1
    SUB_SCENE_2,    // Action scene 2
    SUB_SCENE_3,    // Action scene 3
};

class SceneManager
{
private:
    std::unique_ptr<Scene> m_currentScene;
    SceneType m_currentType = SceneType::FIELD;
    SceneType m_nextType    = SceneType::FIELD;
    bool      m_isChanging  = false;
    GameContext* m_ctx     = nullptr;

public:
    // Register shared DX resources
    void Init(GameContext* ctx);

    // Switch by SceneType directly
    void ChangeScene(SceneType type);

    // Switch to sub-scene by index (0-3)
    void ChangeToSubScene(int index);

    void Update(double deltaTime);
    void Draw();
};

#endif // SCENE_MANAGER_H
