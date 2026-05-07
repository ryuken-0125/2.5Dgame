/*------------------------------
 * shop_manager.h
 * ショップのアイテム管理・購入ロジック
 *------------------------------*/
#pragma once
#include "shop_item.h"
#include <vector>

class ShopManager
{
public:
    ShopManager();

    int  GetCoin() const { return m_coin; }
    void AddCoin(int amount) { m_coin += amount; }

    const std::vector<ShopItem>& GetItems() const { return m_items; }

    // 購入試行（成功 = true / 失敗 = コイン不足 or 在庫切れ）
    bool TryPurchase(int index);

private:
    void InitItems();

    int                   m_coin = 100;
    std::vector<ShopItem> m_items;
};