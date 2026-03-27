#include "CherryBombPlant.hpp"

#include <cmath>

#include "Util/Animation.hpp"
#include "Zombie.hpp"

CherryBombPlant::CherryBombPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, true, 0),
            20.0f,
            300.0f,
            1800.0f,
            0.1f) {}

void CherryBombPlant::Update(float deltaTime) {
    (void)deltaTime;
}

std::optional<PlantProjectile> CherryBombPlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    if (m_HasExploded) {
        return std::nullopt;
    }

    bool zombieInRange = false;
    for (const auto& zombie : zombies) {
        if (!zombie || zombie->IsDead()) {
            continue;
        }
        const float dx = std::abs(zombie->m_Transform.translation.x - m_Transform.translation.x);
        const int rowDiff = std::abs(zombie->GetRow() - GetRow());
        if (rowDiff <= 1 && dx <= 100.0f) {
            zombieInRange = true;
            break;
        }
    }

    if (!zombieInRange || !TryAttack(deltaTime)) {
        return std::nullopt;
    }

    // 3x3 blast: same column plus one column on each side, and adjacent rows.
    for (const auto& zombie : zombies) {
        if (!zombie || zombie->IsDead()) {
            continue;
        }
        const float dx = std::abs(zombie->m_Transform.translation.x - m_Transform.translation.x);
        const int rowDiff = std::abs(zombie->GetRow() - GetRow());
        if (rowDiff <= 1 && dx <= 100.0f) {
            zombie->TakeDamage(GetAttackDamage());
        }
    }

    m_HasExploded = true;
    TakeDamage(GetHealth());
    return std::nullopt;
}
