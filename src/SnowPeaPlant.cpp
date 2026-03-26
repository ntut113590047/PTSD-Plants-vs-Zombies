#include "SnowPeaPlant.hpp"

#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Zombie.hpp"

SnowPeaPlant::SnowPeaPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, true, 0),
            20.0f,
            999.0f,
            20.0f,
            1.5f) {}

void SnowPeaPlant::Update(float deltaTime) {
    (void)deltaTime;
}

std::optional<PlantProjectile> SnowPeaPlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    if (zombies.empty()) {
        return std::nullopt;
    }

    if (!TryAttack(deltaTime)) {
        return std::nullopt;
    }

    auto peaObj = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Plants/coldBeanAttack.png"),
        20.0f
    );
    peaObj->m_Transform.translation = {
        m_Transform.translation.x + 30.0f,
        m_Transform.translation.y + 20.0f
    };
    peaObj->m_Transform.scale = {0.5f, 0.5f};

    return PlantProjectile{peaObj, GetRow(), 500.0f, GetAttackDamage()};
}
