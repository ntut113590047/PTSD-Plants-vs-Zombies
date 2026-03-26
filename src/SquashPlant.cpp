#include "SquashPlant.hpp"

#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Zombie.hpp"

SquashPlant::SquashPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, true, 0),
            20.0f,
            150.0f,
            50.0f,
            4.0f) {}

void SquashPlant::Update(float deltaTime) {
    (void)deltaTime;
}

std::optional<PlantProjectile> SquashPlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    if (!DetectZombieInFront(zombies)) {
        return std::nullopt;
    }

    if (!TryAttack(deltaTime)) {
        return std::nullopt;
    }

    auto squashObj = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Plants/squash.png"),
        25.0f
    );
    squashObj->m_Transform.translation = {
        m_Transform.translation.x + 30.0f,
        m_Transform.translation.y + 20.0f
    };
    squashObj->m_Transform.scale = {0.8f, 0.8f};

    return PlantProjectile{squashObj, GetRow(), 400.0f, GetAttackDamage()};
}
