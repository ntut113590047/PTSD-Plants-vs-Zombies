#include "RepeaterPlant.hpp"

#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Zombie.hpp"

RepeaterPlant::RepeaterPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, true, 0),
            20.0f,
            300.0f,
            20.0f,
            1.4f) {}

void RepeaterPlant::Update(float deltaTime) {
    (void)deltaTime;
}

std::optional<PlantProjectile> RepeaterPlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    if (m_IsSecondShotPending) {
        m_SecondShotTimer += deltaTime;
        if (m_SecondShotTimer >= SECOND_SHOT_DELAY) {
            m_IsSecondShotPending = false;
            m_SecondShotTimer = 0.0f;
            return CreateBeanProjectile();
        }
    }

    if (!DetectZombieInFront(zombies)) {
        return std::nullopt;
    }

    if (!TryAttack(deltaTime)) {
        return std::nullopt;
    }

    m_IsSecondShotPending = true;
    m_SecondShotTimer = 0.0f;
    return CreateBeanProjectile();
}

std::optional<PlantProjectile> RepeaterPlant::CreateBeanProjectile() const {
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
