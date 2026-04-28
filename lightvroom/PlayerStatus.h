#pragma once
class PlayerStatus
{
public:
    PlayerStatus();
    ~PlayerStatus();
    void Initialize();
    void Update(float deltaTime, bool isDashing);

    // --- 状態付与 ---
    void ApplyStun();
    void ApplySlow();

    // --- 状態取得 ---
    bool  IsStunned()          const { return m_stunTimer > 0.0f; }
    bool  IsSlowed()           const { return m_slowTimer > 0.0f; }
    float GetSpeedMultiplier() const { return IsSlowed() ? SLOW_SPEED_RATIO : 1.0f; }

    // --- ステータス比率取得 ---
    float GetHpRatio()     const { return m_hp / m_maxHp; }
    float GetSanityRatio() const { return m_sanity / m_maxSanity; }

private:
    float m_hp;
    float m_maxHp;
    float m_sanity;
    float m_maxSanity;
    float m_stunTimer;  // スタン残り時間（0以下で解除）
    float m_slowTimer;  // 低速残り時間（0以下で解除）

    const float SANITY_DECREASE_SPEED = 1.0f;
    const float DASH_DRAIN_SPEED = 2.0f;
    const float STUN_DURATION = 3.0f;  // スタン持続秒数
    const float SLOW_DURATION = 5.0f;  // 低速持続秒数
    const float SLOW_SPEED_RATIO = 0.5f;  // 低速時の速度倍率
};
