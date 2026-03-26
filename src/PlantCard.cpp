#include "PlantCard.hpp"

PlantCard::PlantCard(const PlantData& data, float x, float y)
    : Util::GameObject(), m_Data(data)
{
    // Use plant animation instead of card image
    if (!data.plantAnimationPaths.empty()) {
        m_Animation = std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 100, true, 0);
        SetDrawable(m_Animation);
    } else {
        // Fallback to single image if animation paths are empty
        m_Animation = std::make_shared<Util::Animation>(
            std::vector<std::string>{data.cardImagePath}, true, 100, true, 0
        );
        SetDrawable(m_Animation);
    }
    
    SetZIndex(10);

    m_Transform.translation.x = x;
    m_Transform.translation.y = y;
    // Scale plant to show in card - use the plant's scale value
    m_Transform.scale = {data.scale * 0.5f, data.scale * 0.5f};

    m_Brightness = 1.0f; // 初始為完全亮度
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

void PlantCard::SetEnergyAvailable(bool available) {
    if (!m_Animation) return;

    if (available) {
        m_Brightness = 1.0f;
    } else {
        m_Brightness = 0.7f; // 調低一點點，而不是太暗
    }
}
