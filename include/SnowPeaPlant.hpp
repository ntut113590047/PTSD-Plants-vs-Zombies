#ifndef SNOW_PEA_PLANT_HPP
#define SNOW_PEA_PLANT_HPP

#include "Plant.hpp"

class SnowPeaPlant : public Plant {
public:
    explicit SnowPeaPlant(const PlantData& data);

    void Update(float deltaTime) override;
    std::optional<PlantProjectile> Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) override;
};

#endif
