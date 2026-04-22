#pragma once
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "EnemyBase.h"

// 敵の種類を定義する列挙型
enum class EnemyType {
    MobA,
    MobB,
    MobC,
    Boss
};

class EnemyManager {
private:
    // 敵の保持用コンテナ（基底クラスのポインタで管理）
    std::vector<std::unique_ptr<EnemyBase>> m_Enemies;

public:
    EnemyManager() = default;
    ~EnemyManager() = default;

    // 更新処理（全敵のUpdate呼び出し）
    void Update(float deltaTime);

    // 描画処理（全敵のDraw呼び出し）
    void Draw();

    // 敵の生成（Factoryメソッド）
    void SpawnEnemy(EnemyType type, DirectX::XMFLOAT3 position);

    // アクティブでない敵の削除
    void RemoveInactiveEnemies();

    // デバッグ用：現在の敵の数を取得
    size_t GetEnemyCount() const { return m_Enemies.size(); }
};