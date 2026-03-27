#include "SunflowerPlant.hpp"

#include "Util/Animation.hpp"

SunflowerPlant::SunflowerPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 80, true, 0),
            20.0f,
            300.0f,
            0.0f,
            24.0f) {}

void SunflowerPlant::Update(float deltaTime) {
    m_SunTimer += deltaTime;
}

bool SunflowerPlant::TryProduceSun() {
    if (m_SunTimer < m_SunInterval) {
        return false;
    }

    m_SunTimer -= m_SunInterval;
    return true;
}

int SunflowerPlant::GetSunProductionAmount() const {
    return m_SunAmount;
}
