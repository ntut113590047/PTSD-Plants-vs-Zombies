#include "CherryBombPlant.hpp"

#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Zombie.hpp"

CherryBombPlant::CherryBombPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, true, 0),
            20.0f,
            200.0f,
            100.0f,
            6.0f) {}

void CherryBombPlant::Update(float deltaTime) {
    (void)deltaTime;
}

std::optional<PlantProjectile> CherryBombPlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    if (!DetectZombieInFront(zombies)) {
        return std::nullopt;
    }

    if (!TryAttack(deltaTime)) {
        return std::nullopt;
    }

    auto cherryObj = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Plants/cherry.png"),
        25.0f
    );
    cherryObj->m_Transform.translation = {
        m_Transform.translation.x + 30.0f,
        m_Transform.translation.y + 20.0f
    };
    cherryObj->m_Transform.scale = {0.8f, 0.8f};

    return PlantProjectile{cherryObj, GetRow(), 300.0f, GetAttackDamage()};
}
