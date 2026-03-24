#include "PlantCard.hpp"

PlantCard::PlantCard(const PlantData& data, float x, float y)
    : Util::GameObject(std::make_shared<Util::Image>(data.cardImagePath), 10), m_Data(data)
{
    m_Transform.translation.x = x;
    m_Transform.translation.y = y;
    m_Transform.scale = {0.25f, 0.25f};
}

void PlantCard::Follow(float parentY) {
    m_Transform.translation.y = parentY;
}

void PlantCard::UpdateCooldown(float deltaTime) {
    if (m_IsOnCooldown) {
        m_CooldownTimer -= deltaTime;
        if (m_CooldownTimer <= 0.0f) {
            m_IsOnCooldown = false;
            m_CooldownTimer = 0.0f;
        }
    }
}

void PlantCard::StartCooldown() {
    m_IsOnCooldown = true;
    m_CooldownTimer = m_Data.cooldown;
}