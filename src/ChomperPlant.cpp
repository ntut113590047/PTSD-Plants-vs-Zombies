#include "ChomperPlant.hpp"

#include <cmath>

#include "Util/Animation.hpp"
#include "Zombie.hpp"

ChomperPlant::ChomperPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, true, 0),
            20.0f,
            300.0f,
            1800.0f,
            42.0f) {}

void ChomperPlant::Update(float deltaTime) {
    (void)deltaTime;
}

bool ChomperPlant::DetectZombieInFront(const std::vector<std::shared_ptr<Zombie>>& zombies) const {
    for (const auto& zombie : zombies) {
        if (!zombie || zombie->IsDead() || zombie->GetRow() != GetRow()) {
            continue;
        }

        const float dx = zombie->m_Transform.translation.x - m_Transform.translation.x;
        if (dx >= 0.0f && dx <= 100.0f) {
            return true;
        }
    }

    return false;
}

std::optional<PlantProjectile> ChomperPlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    if (!DetectZombieInFront(zombies)) {
        return std::nullopt;
    }

    if (!TryAttack(deltaTime)) {
        return std::nullopt;
    }

    std::shared_ptr<Zombie> target = nullptr;
    float nearestDx = 999999.0f;
    for (const auto& zombie : zombies) {
        if (!zombie || zombie->IsDead() || zombie->GetRow() != GetRow()) {
            continue;
        }

        const float dx = zombie->m_Transform.translation.x - m_Transform.translation.x;
        if (dx >= 0.0f && dx <= 100.0f && dx < nearestDx) {
            nearestDx = dx;
            target = zombie;
        }
    }

    if (target) {
        target->TakeDamage(GetAttackDamage());
    }

    return std::nullopt;
}
