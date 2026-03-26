#include "ChomperPlant.hpp"

#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Zombie.hpp"

ChomperPlant::ChomperPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, true, 0),
            150.0f,
            999.0f,
            100.0f,
            15.0f) {}

void ChomperPlant::Update(float deltaTime) {
    (void)deltaTime;
}

std::optional<PlantProjectile> ChomperPlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    if (!DetectZombieInFront(zombies)) {
        return std::nullopt;
    }

    if (!TryAttack(deltaTime)) {
        return std::nullopt;
    }

    auto chomperObj = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Plants/chomper.png"),
        25.0f
    );
    chomperObj->m_Transform.translation = {
        m_Transform.translation.x + 30.0f,
        m_Transform.translation.y + 20.0f
    };
    chomperObj->m_Transform.scale = {0.8f, 0.8f};

    return PlantProjectile{chomperObj, GetRow(), 0.0f, GetAttackDamage()};
}
