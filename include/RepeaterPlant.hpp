#ifndef REPEATER_PLANT_HPP
#define REPEATER_PLANT_HPP

#include "Plant.hpp"

class RepeaterPlant : public Plant {
public:
    explicit RepeaterPlant(const PlantData& data);

    void Update(float deltaTime) override;
    std::optional<PlantProjectile> Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) override;

private:
    std::optional<PlantProjectile> CreateBeanProjectile() const;

    bool m_IsSecondShotPending = false;
    float m_SecondShotTimer = 0.0f;
    static constexpr float SECOND_SHOT_DELAY = 0.2f;
};

#endif
