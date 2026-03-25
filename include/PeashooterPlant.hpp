#ifndef PEASHOOTER_PLANT_HPP
#define PEASHOOTER_PLANT_HPP

#include "Plant.hpp"

class PeashooterPlant : public Plant {
public:
    explicit PeashooterPlant(const PlantData& data);

    void Update(float deltaTime) override;
    std::optional<PlantProjectile> Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) override;
};

#endif