/*------------------------------
 * shop_manager.cpp
 *------------------------------*/
#include "shop_manager.h"

ShopManager::ShopManager()
{
    InitItems();
}

void ShopManager::InitItems()
{
    //         name         price  stock   color(RGBA)                        isNew
    m_items =
    {
        { "HP回復(小)",    10,   -1,  { 0.20f, 0.80f, 0.20f, 1.0f },  false },
        { "HP回復(大)",    30,   -1,  { 0.10f, 0.55f, 0.10f, 1.0f },  false },
        { "スタン解除",    20,    5,  { 0.80f, 0.60f, 0.10f, 1.0f },  true  },
        { "移動速UP",      50,    3,  { 0.20f, 0.40f, 1.00f, 1.0f },  true  },
        { "時限BP+12",     35,   -1,  { 1.00f, 0.80f, 0.10f, 1.0f },  true  },
        { "時限BP+6",      20,   -1,  { 1.00f, 0.60f, 0.10f, 1.0f },  true  },
        { "セット[赤]",    50,   -1,  { 0.90f, 0.20f, 0.20f, 1.0f },  true  },
        { "正気度回復",     4,   -1,  { 0.60f, 0.20f, 0.90f, 1.0f },  false },
    };
}

bool ShopManager::TryPurchase(int index)
{
    if (index < 0 || index >= static_cast<int>(m_items.size())) return false;

    ShopItem& item = m_items[index];
    if (item.stock == 0)     return false;   // 在庫切れ
    if (m_coin < item.price) return false;   // コイン不足

    m_coin -= item.price;
    if (item.stock > 0) --item.stock;
    return true;
}