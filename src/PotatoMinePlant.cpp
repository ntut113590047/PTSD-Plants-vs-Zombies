#include "PotatoMinePlant.hpp"

#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Zombie.hpp"

PotatoMinePlant::PotatoMinePlant(const PlantData& data,
                                 const std::shared_ptr<Core::Drawable>& drawable,
                                 float zIndex)
    : Plant(data,
            drawable != nullptr ? drawable : std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, true, 0),
            zIndex,
        300.0f,
        1800.0f,
            0.0f) {}

void PotatoMinePlant::Update(float deltaTime) {
    if (m_HasExploded) {
        return;
    }

    m_ExplodeTimer += deltaTime;
}

std::optional<PlantProjectile> PotatoMinePlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    (void)deltaTime;
    if (m_HasExploded) {
        return std::nullopt;
    }

    if (m_ExplodeTimer < m_ExplodeDelay) {
        return std::nullopt;
    }

    if (!DetectZombieInFront(zombies)) {
        return std::nullopt;
    }

    m_HasExploded = true;

    auto explosionObj = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Plants/potatoMineExplosion.png"),
        25.0f
    );
    explosionObj->m_Transform.translation = {
        m_Transform.translation.x + 30.0f,
        m_Transform.translation.y + 20.0f
    };
    explosionObj->m_Transform.scale = {0.9f, 0.9f};

    return PlantProjectile{explosionObj, GetRow(), 0.0f, GetAttackDamage()};
}
