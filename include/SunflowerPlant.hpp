#ifndef SUNFLOWER_PLANT_HPP
#define SUNFLOWER_PLANT_HPP

#include "Plant.hpp"

class SunflowerPlant : public Plant {
public:
    explicit SunflowerPlant(const PlantData& data);

    void Update(float deltaTime) override;
    bool TryProduceSun();
    int GetSunProductionAmount() const;

private:
    float m_SunTimer = 0.0f;
    float m_SunInterval = 24.0f;
    int m_SunAmount = 25;
};

#endif