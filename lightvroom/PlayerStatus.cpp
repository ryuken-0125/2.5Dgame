#include "PlayerStatus.h"

PlayerStatus::PlayerStatus()
    : m_hp(100.0f)
    , m_maxHp(100.0f)
    , m_sanity(100.0f)
    , m_maxSanity(100.0f)
{
}

PlayerStatus::~PlayerStatus()
{
}

void PlayerStatus::Initialize()
{
    m_hp = m_maxHp;
    m_sanity = m_maxSanity;
}

void PlayerStatus::Update(float deltaTime, bool isDashing)
{
    float drainAmount = SANITY_DECREASE_SPEED;

    //ダッシュを追加した時用
    if (isDashing)
    {
        drainAmount += DASH_DRAIN_SPEED;
    }

    m_sanity -= drainAmount * deltaTime;

    if (m_sanity < 0.0f)
    {
        m_sanity = 0.0f;
    }
}