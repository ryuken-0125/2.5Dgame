#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <memory>
#include "scene.h"

// ÉVÅ[ÉìÇÃéÌóﬁ
enum class SceneType
{
    TITLE,
    GAME,
    RESULT
};

class SceneManager
{
private:
    std::unique_ptr<Scene> currentScene;

    SceneType currentType;
    SceneType nextSceneType;
    bool isChanging = false;

public:
    void ChangeScene(SceneType type);

    void Update(double deltaTime);
    void Draw();
};

#endif // SCENE_MANAGER_H