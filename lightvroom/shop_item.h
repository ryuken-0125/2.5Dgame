/*------------------------------
 * shop_item.h
 * ショップアイテムのデータ定義
 *------------------------------*/
#pragma once
#include <string>
#include <DirectXMath.h>

struct ShopItem
{
    std::string           name;   // アイテム名
    int                   price;  // 価格（コイン）
    int                   stock;  // 在庫数（-1 = 無制限）
    DirectX::XMFLOAT4     color;  // スロット表示色
    bool                  isNew;  // "NEW" バッジ表示フラグ
};