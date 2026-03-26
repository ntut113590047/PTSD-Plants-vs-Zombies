#ifndef PLANTCARD_HPP
#define PLANTCARD_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <string>
#include <memory>
#include <vector>

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
    
    // 獲取當前明度
    float GetBrightness() const { return m_Brightness; }
    
    // 獲取冷卻進度 (0.0 = 完全冷卻，1.0 = 完全恢復)
    float GetCooldownProgress() const {
        if (m_Data.cooldown <= 0.0f) return 1.0f;
        return 1.0f - (m_CooldownTimer / m_Data.cooldown);
    }
    
    // 是否在冷卻中
    bool IsCoolingDown() const { return m_IsOnCooldown; }

private:
    PlantData m_Data;
    bool m_IsOnCooldown = false;
    float m_CooldownTimer = 0.0f;
    float m_Brightness = 1.0f; // 明度狀態（1.0 = 正常，0.7 = 不可用）
    std::shared_ptr<Util::Image> m_CardImage;
};

#endif
