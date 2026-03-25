#include "SunflowerPlant.hpp"

#include "Util/Animation.hpp"

SunflowerPlant::SunflowerPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 80, true, 0),
            20.0f,
            110.0f,
            8.0f,
            1.5f) {}

void SunflowerPlant::Update(float deltaTime) {
    (void)deltaTime;
}
