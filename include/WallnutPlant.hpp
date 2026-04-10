#ifndef WALNUT_PLANT_HPP
#define WALNUT_PLANT_HPP

#include "Plant.hpp"

namespace Util {
class Animation;
}

class WallnutPlant : public Plant {
public:
    WallnutPlant(const PlantData& data,
                 const std::shared_ptr<Core::Drawable>& drawable = nullptr,
                 float zIndex = 20.0f,
                 float health = 4000.0f);

    void Update(float deltaTime) override;
    void TakeDamage(float damage) override;

    bool DetectZombieInFront(const std::vector<std::shared_ptr<Zombie>>& zombies) const override;
    std::optional<PlantProjectile> Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) override;

private:
    void UpdateDamageVisual();

private:
    std::shared_ptr<Util::Animation> m_WallnutStage1Animation;
    std::shared_ptr<Util::Animation> m_WallnutStage2Animation;
    std::shared_ptr<Util::Animation> m_WallnutStage3Animation;
    int m_CurrentVisualStage = 1;
};

#endif
