#ifndef SQUASH_PLANT_HPP
#define SQUASH_PLANT_HPP

#include "Plant.hpp"

class SquashPlant : public Plant {
public:
    explicit SquashPlant(const PlantData& data);

    void Update(float deltaTime) override;
    std::optional<PlantProjectile> Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) override;
};

#endif
