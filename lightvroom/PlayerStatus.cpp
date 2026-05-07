#include "PlayerStatus.h"

PlayerStatus::PlayerStatus()
    : m_hp(100.0f)
    , m_maxHp(100.0f)
    , m_sanity(100.0f)
    , m_maxSanity(100.0f)
    , m_stunTimer(0.0f)
    , m_slowTimer(0.0f)
{
}

PlayerStatus::~PlayerStatus()
{
}

void PlayerStatus::Initialize()
{
    m_hp = m_maxHp;
    m_sanity = m_maxSanity;
    m_stunTimer = 0.0f;
    m_slowTimer = 0.0f;
}

void PlayerStatus::Update(float deltaTime, bool isDashing)
{
    // --- スタンタイマー更新 ---
    if (m_stunTimer > 0.0f)
    {
        m_stunTimer -= deltaTime;
        if (m_stunTimer < 0.0f) m_stunTimer = 0.0f;
    }

    // --- 低速タイマー更新 ---
    if (m_slowTimer > 0.0f)
    {
        m_slowTimer -= deltaTime;
        if (m_slowTimer < 0.0f) m_slowTimer = 0.0f;
    }

    // --- 正気度減少 ---
    float drainAmount = SANITY_DECREASE_SPEED;
    if (isDashing)
    {
        drainAmount += DASH_DRAIN_SPEED;
    }
    m_sanity -= drainAmount * deltaTime;
    if (m_sanity < 0.0f) m_sanity = 0.0f;
}

/*====================================================================
 * スタン付与
 * タイマーをリセットして3秒間移動不能にする
 *====================================================================*/
void PlayerStatus::ApplyStun()
{
    m_stunTimer = STUN_DURATION;
}

/*====================================================================
 * 低速付与
 * タイマーをリセットして5秒間速度を半減させる
 *====================================================================*/
void PlayerStatus::ApplySlow()
{
    m_slowTimer = SLOW_DURATION;
}
