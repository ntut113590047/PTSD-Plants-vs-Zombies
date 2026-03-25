#include "PlantCard.hpp"

PlantCard::PlantCard(const PlantData& data, float x, float y)
    : Util::GameObject(), m_Data(data)
{
    m_CardImage = std::make_shared<Util::Image>(data.cardImagePath);
    SetDrawable(m_CardImage);
    SetZIndex(10);

    m_Transform.translation.x = x;
    m_Transform.translation.y = y;
    m_Transform.scale = {0.25f, 0.25f};

    m_CardImage->SetBrightness(1.0f);
    m_CardImage->SetCooldownFill(0.0f);
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

    float cooldownFill = 0.0f;
    if (m_Data.cooldown > 0.0f && m_IsOnCooldown) {
        cooldownFill = m_CooldownTimer / m_Data.cooldown;
        cooldownFill = std::clamp(cooldownFill, 0.0f, 1.0f);
    }

    if (m_CardImage) {
        m_CardImage->SetCooldownFill(cooldownFill);
    }
}

void PlantCard::StartCooldown() {
    m_IsOnCooldown = true;
    m_CooldownTimer = m_Data.cooldown;
}

void PlantCard::SetEnergyAvailable(bool available) {
    if (!m_CardImage) return;

    if (available) {
        m_CardImage->SetBrightness(1.0f);
    } else {
        m_CardImage->SetBrightness(0.55f);
    }
}
