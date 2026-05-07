#pragma once
#include <DirectXMath.h>
#include <memory>
#include "collision.h" // 既存の衝突判定を利用

// 敵の基底クラス
class EnemyBase {
protected:
    // 命名規則：メンバ変数には m_ を付与 
    DirectX::XMFLOAT3 m_Position{};
    DirectX::XMFLOAT3 m_Velocity{};

    // コンポジション：当たり判定は「継承」ではなく「保持」する方が柔軟です [cite: 127]
    std::unique_ptr<Collision> m_Collider;

    bool m_IsActive{ true };

public:
    virtual ~EnemyBase() = default;

    // 純粋仮想関数：派生クラス（Goblin, Slimeなど）で必ず実装させる
    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;

    // 衝突判定の共通処理
    virtual void OnCollision(const Hit& hit) {
        // 衝突した時の挙動（押し返し処理など）をここで定義
    }

    // Setter/Getter
    void SetPosition(const DirectX::XMFLOAT3& pos) { m_Position = pos; }
    const DirectX::XMFLOAT3& GetPosition() const { return m_Position; }

    bool IsActive() const { return m_IsActive; }
};

// 雑魚敵クラス (それぞれの個性を実装)
class MobEnemyA : public EnemyBase { void Update(float dt) override; }; // 例：突進してくる
class MobEnemyB : public EnemyBase { void Update(float dt) override; }; // 例：弾を撃つ
class MobEnemyC : public EnemyBase { void Update(float dt) override; }; // 例：動かない

// BOSSクラス
class BossEnemy : public EnemyBase { void Update(float dt) override; };