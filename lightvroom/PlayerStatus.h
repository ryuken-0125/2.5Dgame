#pragma once

class PlayerStatus
{
public:
    PlayerStatus();
    ~PlayerStatus();

    void Initialize();
    void Update(float deltaTime, bool isDashing);

    // 割合（0.0 ～ 1.0）を返すゲッター
    float GetHpRatio() const { return m_hp / m_maxHp; }
    float GetSanityRatio() const { return m_sanity / m_maxSanity; }

private:
    float m_hp;
    float m_maxHp;
    float m_sanity;
    float m_maxSanity;

    const float SANITY_DECREASE_SPEED = 1.0f;
    const float DASH_DRAIN_SPEED = 2.0f;
};