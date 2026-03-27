#include "PeashooterPlant.hpp"

#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Zombie.hpp"

PeashooterPlant::PeashooterPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, true, 0),
            20.0f,
            300.0f,
            20.0f,
            1.5f) {}

void PeashooterPlant::Update(float deltaTime) {
    (void)deltaTime;
}

std::optional<PlantProjectile> PeashooterPlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    if (!DetectZombieInFront(zombies)) {
        return std::nullopt;
    }

    if (!TryAttack(deltaTime)) {
        return std::nullopt;
    }

    auto beanObj = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Plants/beanAttack.png"),
        25.0f
    );
    beanObj->m_Transform.translation = {
        m_Transform.translation.x + 30.0f,
        m_Transform.translation.y + 20.0f
    };
    beanObj->m_Transform.scale = {0.8f, 0.8f};

    return PlantProjectile{beanObj, GetRow(), 300.0f, GetAttackDamage()};
}
