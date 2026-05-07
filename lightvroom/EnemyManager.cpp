#include "EnemyManager.h"
//#include "MobEnemyA.h"
// 必要に応じて他の敵ヘッダーもインクルード

void EnemyManager::SpawnEnemy(EnemyType type, DirectX::XMFLOAT3 position) {
    std::unique_ptr<EnemyBase> newEnemy;

    switch (type) {
    //case EnemyType::MobA:
    //    newEnemy = std::make_unique<MobEnemyA>();
    //    break;d
        // 他の敵クラスもここに追加
    }

    if (newEnemy) {
        newEnemy->SetPosition(position);
        m_Enemies.push_back(std::move(newEnemy));
    }
}

void EnemyManager::Update(float deltaTime) {
    for (auto& enemy : m_Enemies) {
        enemy->Update(deltaTime);
    }
}

void EnemyManager::Draw() {
    for (auto& enemy : m_Enemies) {
        enemy->Draw();
    }
}

void EnemyManager::RemoveInactiveEnemies() {
    // アクティブでない（IsActiveがfalseの）敵をvectorから削除
    m_Enemies.erase(
        std::remove_if(m_Enemies.begin(), m_Enemies.end(),
            [](const std::unique_ptr<EnemyBase>& e) { return !e->IsActive(); }),
        m_Enemies.end()
    );
}


//void GameScene::Update(float deltaTime) {
//    // 1. 敵の動きを更新
//    m_EnemyManager->Update(deltaTime);
//
//    // 2. 衝突判定などを行い、倒されたら敵の m_IsActive = false にする
//    // (省略: ここで衝突判定を行い、必要ならActiveフラグを切り替える)
//
//    // 3. 不要になった敵を削除
//    m_EnemyManager->RemoveInactiveEnemies();
//}
//
//void GameScene::Draw() {
//    // 4. 描画
//    m_EnemyManager->Draw();
//}