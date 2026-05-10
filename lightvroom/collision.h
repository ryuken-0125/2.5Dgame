/*------------------------------
*	当たり判定処理[collision.h]
*
* 制作者：山室飛龍		日付：2025/11/18
------------------------------*/

#ifndef COLLISION_H
#define COLLISION_H

#include <DirectXMath.h>
using namespace DirectX;

class AABB;
class Point;

/*==============================================================
 * 衝突結果を表す構造体クラス
 * ・衝突したかどうか(bool)
 * ・衝突面の法線ベクトル
 *==============================================================*/
class Hit
{
private:
	bool m_isHit{ false };			    // 衝突しているか
	DirectX::XMFLOAT3 m_normal{};	    // 衝突面の法線

public:
	Hit() = default;

	// 衝突結果と法線をセットするコンストラクタ
	Hit(bool isHit, const DirectX::XMFLOAT3& normal)
		:m_isHit(isHit), m_normal{ normal } {
	}

	// 衝突しているか取得
	bool IsHit() const { return m_isHit; };

	void SetNormal(const DirectX::XMFLOAT3& normal) {
		m_normal = normal;
	}

	// 衝突(押し返し)方向となる法線ベクトルを取得
	const DirectX::XMFLOAT3& GetNormal()const {
		return m_normal;
	}
};

/*==============================================================
 * 当たり判定の基底クラス
 * 派生クラスで形状ごとの判定処理を実装する
 *==============================================================*/
class Collision
{
public:
	virtual ~Collision() = default;

	// 指定されたAABBと重なっているか（派生で実装）
	virtual bool IsOverlap(const AABB& aabb) const{ return false; };

	// 指定されたAABBと衝突している場合、衝突情報を返す
	virtual Hit IsHit(const AABB& aabb)const { return {}; };

	virtual bool IsOverlap(const Point&) const { return false; }
	virtual Hit IsHit(const Point&) const { return {}; }
};

/*==============================================================
 * AABB（軸平行境界ボックス）による衝突判定
 * ・Min：最小座標（左下手前）
 * ・Max：最大座標（右上奥）
 *==============================================================*/
class AABB : public Collision
{
private:
	DirectX::XMFLOAT3 m_half{};
	DirectX::XMFLOAT3 m_center{};
	DirectX::XMFLOAT3 m_min{};
	DirectX::XMFLOAT3 m_max{};

public:
	AABB() = default;
	AABB(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max)
		: m_half((max.x - min.x) * 0.5f, (max.y - min.y) * 0.5f, (max.z - min.z) * 0.5f)
		, m_center(min.x + m_half.x, min.y + m_half.y, min.z + m_half.z)
		, m_min(min), m_max(max) {
	}

	static AABB Make(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& size) {
		return {
		 { center.x - size.x * 0.5f, center.y - size.y * 0.5f, center.z - size.z * 0.5f },
		 { center.x + size.x * 0.5f, center.y + size.y * 0.5f, center.z + size.z * 0.5f }
		};
	}

	void Move(const DirectX::XMFLOAT3& position);
	void Move(const DirectX::XMVECTOR& position);

	const DirectX::XMFLOAT3& GetMin() const { return m_min; }
	const DirectX::XMFLOAT3& GetMax() const { return m_max; }
	const DirectX::XMFLOAT3& GetCenter() const { return m_center; }
	const DirectX::XMFLOAT3& GetHalfSize() const { return m_half; }

	bool IsOverlap(const AABB&) const override;
	Hit IsHit(const AABB&) const override;

	bool IsOverlap(const Point& point) const override;

};

class Point : public Collision
{
private:
	DirectX::XMFLOAT3 m_position;

public:
	Point() = default;
	Point(const DirectX::XMFLOAT3& position)
		: m_position(position){
	}

	const DirectX::XMFLOAT3& GetPosition()const { return m_position; }

	bool IsOverlap(const AABB&) const override;
};

#endif // !COLLISION_H
