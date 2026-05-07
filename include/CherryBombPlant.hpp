#ifndef CHERRY_BOMB_PLANT_HPP
#define CHERRY_BOMB_PLANT_HPP

#include "Plant.hpp"

class CherryBombPlant : public Plant {
public:
    explicit CherryBombPlant(const PlantData& data);

    void Update(float deltaTime) override;
    std::optional<PlantProjectile> Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) override;

private:
    bool m_HasExploded = false;
    bool m_HasScheduledRemoval = false;
    float m_TimeSincePlanted = 0.0f;
};

#endif
