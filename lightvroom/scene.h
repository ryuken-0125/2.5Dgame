#ifndef SCENE_H
#define SCENE_H

class SceneManager;

// シーンの基底クラス
class Scene
{
protected:
    SceneManager& m_sceneManager; // シーンからマネージャにアクセス

public:
    Scene(SceneManager& sm) : m_sceneManager(sm) {}
    virtual ~Scene() = default;

    virtual void Initialize() {}
    virtual void Finalize() {}

    virtual void Update(double deltaTime) = 0;
    virtual void Draw() = 0;
};

#endif // SCENE_H