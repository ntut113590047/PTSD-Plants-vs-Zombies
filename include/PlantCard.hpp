#ifndef PLANTCARD_HPP
#define PLANTCARD_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <string>

struct PlantData {
    std::string name;
    int cost;
    float cooldown;
    std::string cardImagePath;
    std::vector<std::string> plantAnimationPaths; // 植物動畫幀路徑
    float scale; // 植物顯示大小
};
class PlantCard : public Util::GameObject {
public:
    PlantCard(const PlantData& data, float x, float y);

    void Follow(float parentY);
    void UpdateCooldown(float deltaTime);
    void StartCooldown();
    bool IsReady() const { return !m_IsOnCooldown; }
    const PlantData& GetData() const { return m_Data; }

    void SetEnergyAvailable(bool available);

private:
    PlantData m_Data;
    bool m_IsOnCooldown = false;
    float m_CooldownTimer = 0.0f;
    std::shared_ptr<Util::Image> m_CardImage;
};

#endif