#include "RepeaterPlant.hpp"

#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Zombie.hpp"

RepeaterPlant::RepeaterPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, true, 0),
            20.0f,
            140.0f,
            20.0f,
            1.0f) {}

void RepeaterPlant::Update(float deltaTime) {
    (void)deltaTime;
}

std::optional<PlantProjectile> RepeaterPlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    if (!DetectZombieInFront(zombies)) {
        m_HasFiredSecondProjectile = false;
        return std::nullopt;
    }

    if (!TryAttack(deltaTime)) {
        return std::nullopt;
    }

    if (!m_HasFiredSecondProjectile) {
        m_HasFiredSecondProjectile = true;
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
    } else {
        m_HasFiredSecondProjectile = false;
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
}
