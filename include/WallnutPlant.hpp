#ifndef WALNUT_PLANT_HPP
#define WALNUT_PLANT_HPP

#include "Plant.hpp"

class WallnutPlant : public Plant {
public:
    WallnutPlant(const PlantData& data,
                 const std::shared_ptr<Core::Drawable>& drawable = nullptr,
                 float zIndex = 20.0f,
                 float health = 180.0f);

    bool DetectZombieInFront(const std::vector<std::shared_ptr<Zombie>>& zombies) const override;
    std::optional<PlantProjectile> Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) override;
};

#endif
