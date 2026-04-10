#ifndef SUNFLOWER_PLANT_HPP
#define SUNFLOWER_PLANT_HPP

#include <memory>

#include "Plant.hpp"

namespace Util {
class Animation;
}

class SunflowerPlant : public Plant {
public:
    explicit SunflowerPlant(const PlantData& data);

    void Update(float deltaTime) override;
    bool TryProduceSun();
    int GetSunProductionAmount() const;

private:
    void UpdateBrightAnimation(float deltaTime);

private:
    float m_SunTimer = 0.0f;
    float m_SunInterval = 24.0f;
    int m_SunAmount = 25;

    std::shared_ptr<Util::Animation> m_NormalAnimation;
    std::shared_ptr<Util::Animation> m_BrightAnimation;
    bool m_IsShowingBright = false;
    float m_BrightReturnTimer = 0.0f;

    float m_PreSunShowBrightDuration = 3.0f;
    float m_PostSunReturnDelay = 1.0f;
};

#endif
