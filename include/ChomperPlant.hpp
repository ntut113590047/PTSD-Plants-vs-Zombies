#ifndef CHOMPER_PLANT_HPP
#define CHOMPER_PLANT_HPP

#include "Plant.hpp"

class ChomperPlant : public Plant {
public:
    explicit ChomperPlant(const PlantData& data);

    void Update(float deltaTime) override;
    bool DetectZombieInFront(const std::vector<std::shared_ptr<Zombie>>& zombies) const override;
    std::optional<PlantProjectile> Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) override;
};

#endif
