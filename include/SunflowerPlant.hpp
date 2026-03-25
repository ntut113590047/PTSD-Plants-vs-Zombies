#ifndef SUNFLOWER_PLANT_HPP
#define SUNFLOWER_PLANT_HPP

#include "Plant.hpp"

class SunflowerPlant : public Plant {
public:
    explicit SunflowerPlant(const PlantData& data);

    void Update(float deltaTime) override;
};

#endif