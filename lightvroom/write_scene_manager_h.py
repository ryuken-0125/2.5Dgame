import sys

content = r"""#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <memory>
#include "scene.h"

struct GameContext;

enum class SceneType
{
    FIELD,
    SUB_SCENE_0,
    SUB_SCENE_1,
    SUB_SCENE_2,
    SUB_SCENE_3,
};

class SceneManager
{
private:
    std::unique_ptr<Scene> currentScene;
    SceneType currentType  = SceneType::FIELD;
    SceneType nextType     = SceneType::FIELD;
    bool      isChanging   = false;
    GameContext* m_ctx     = nullptr;

public:
    void Init(GameContext* ctx);
    void ChangeScene(SceneType type);
    void ChangeToSubScene(int index);
    void Update(double deltaTime);
    void Draw();
};

#endif // SCENE_MANAGER_H
"""

path = r'c:\Users\honoka.T\Desktop\GameJam\2.5Dgame\lightvroom\scene_manager.h'
with open(path, 'w', encoding='utf-8') as f:
    f.write(content)
print('scene_manager.h written OK')
