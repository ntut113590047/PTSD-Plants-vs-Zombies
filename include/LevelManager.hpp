#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include <memory>
#include <vector>
#include <string>
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"
#include "Util/Text.hpp"
#include "Button.hpp"
#include "PlantCard.hpp"
#include "glm/vec2.hpp"

class LevelManager {
public:
    LevelManager(int level);

    void LoadLevel(Util::Renderer& root);
    void Update(Util::Renderer& root);
    void ChangeLevel(int level, Util::Renderer& root);

private:
    bool IsGameLevel() const;

private:
    int m_CurrentLevel;

    // 背景
    std::shared_ptr<Util::GameObject> m_Background;

    // 按鈕
    std::vector<std::shared_ptr<Button>> m_Buttons;

    // 🔤 文字動畫
    int m_WordPhase = 0;
    int m_WordTimer = 0;
    std::shared_ptr<Util::GameObject> m_Word;

    // 🧩 卡片槽
    std::shared_ptr<Util::GameObject> m_CardSlot;
    bool m_CardSlotActive = false;
    float m_CardSlotTargetY = 315.0f; // 卡片槽目標位置

    // 🃏 卡片
    std::vector<std::shared_ptr<PlantCard>> m_Cards;
    std::vector<glm::vec2> m_CardPositions; // 卡片在槽內的座標
    std::vector<PlantData> m_LevelPlants;   // 關卡植物資料

    // 遊戲狀態
    int m_PlayerEnergy = 50; // 玩家能量
    std::vector<std::vector<bool>> m_GrassGrid; // 草坪網格，true表示被佔用
    std::vector<std::shared_ptr<Util::GameObject>> m_PlacedPlants; // 已放置的植物
    std::shared_ptr<PlantCard> m_SelectedCard = nullptr; // 當前選擇的卡片
    std::shared_ptr<Util::GameObject> m_FollowingPlant = nullptr; // 跟隨滑鼠的植物圖片
    std::shared_ptr<Util::GameObject> m_PreviewPlant = nullptr; // 預覽植物（半透明）
    std::shared_ptr<Util::GameObject> m_EnergyText = nullptr; // 能量顯示文字
    std::shared_ptr<Util::Text> m_EnergyTextPtr = nullptr;

    struct SunEnergy {
        std::shared_ptr<Util::GameObject> object;
        glm::vec2 velocity;
        bool collecting = false;
        float stopY = -150.0f; // 隨機停止高度
    };

    std::vector<SunEnergy> m_SunEnergies;
    int m_SunSpawnTimer = 0; // 計時器（幀）

    // 開場動畫控制
    bool m_IntroDone = false;
};

#endif