#ifndef POTATO_MINE_PLANT_HPP
#define POTATO_MINE_PLANT_HPP

#include "Plant.hpp"

class PotatoMinePlant : public Plant {
public:
    PotatoMinePlant(const PlantData& data,
                    const std::shared_ptr<Core::Drawable>& drawable = nullptr,
                    float zIndex = 20.0f);

    void Update(float deltaTime) override;
    std::optional<PlantProjectile> Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) override;

private:
    float m_ExplodeTimer = 0.0f;
    float m_ExplodeDelay = 3.0f;
    bool m_HasExploded = false;
};

#endif
