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
#include "Plant.hpp"
#include "Zombie.hpp"
#include "LawnMower.hpp"
#include "LevelConfig.hpp"
#include "LevelConfigParser.hpp"
#include "GameStateManager.hpp"
#include "glm/vec2.hpp"

// 配置常量
namespace LevelManagerConfig {
    constexpr int NUM_CARDS = 6;
    constexpr float CARDSLOT_TARGET_Y = 315.0f;
    constexpr float SUN_STOPPING_HEIGHT = -150.0f;
    constexpr float BEAN_SPEED = 260.0f;
    constexpr float BEAN_DAMAGE = 20.0f;
    constexpr int INITIAL_PLAYER_ENERGY = 50;
}

class LevelManager {
public:
    LevelManager(int level);

    void LoadLevel(Util::Renderer& root);
    void Update(Util::Renderer& root, float deltaTime);
    void ChangeLevel(int level, Util::Renderer& root);

private:
    bool IsGameLevel() const;
    std::shared_ptr<Zombie> SpawnZombie(const std::string& zombieType, int row, float y);

private:
    int m_CurrentLevel;

    // 背景
    std::shared_ptr<Util::GameObject> m_Background;

    // 按鈕
    std::vector<std::shared_ptr<Button>> m_Buttons;

    // 🔤 文字動畫
    int m_WordPhase = 0;
    float m_WordTimer = 0.0f;
    std::shared_ptr<Util::GameObject> m_Word;

    // 🧩 卡片槽
    std::shared_ptr<Util::GameObject> m_CardSlot;
    bool m_CardSlotActive = false;
    float m_CardSlotTargetY = LevelManagerConfig::CARDSLOT_TARGET_Y;

    // 🃏 卡片
    std::vector<std::shared_ptr<PlantCard>> m_Cards;
    std::vector<glm::vec2> m_CardPositions; // 卡片在槽內的座標
    std::vector<PlantData> m_LevelPlants;   // 關卡植物資料
    struct CardVisuals {
        std::shared_ptr<Util::GameObject> energyDimOverlay;
        std::shared_ptr<Util::GameObject> cooldownOverlay;
    };
    std::vector<CardVisuals> m_CardVisuals;

    // 遊戲狀態
    int m_PlayerEnergy = LevelManagerConfig::INITIAL_PLAYER_ENERGY;
    std::vector<std::vector<bool>> m_GrassGrid; // 草坪網格，true表示被佔用
    std::vector<bool> m_RowAllowed; // 每行是否允許放置植物
    std::vector<std::shared_ptr<LawnMower>> m_LawnMowers; // 每行割草機
    std::vector<std::shared_ptr<Plant>> m_PlacedPlants; // 已放置的植物
    std::shared_ptr<PlantCard> m_SelectedCard = nullptr; // 當前選擇的卡片
    std::shared_ptr<Util::GameObject> m_FollowingPlant = nullptr; // 跟隨滑鼠的植物圖片
    std::shared_ptr<Util::GameObject> m_PreviewPlant = nullptr; // 預覽植物（半透明）
    std::shared_ptr<Util::GameObject> m_EnergyText = nullptr; // 能量顯示文字
    std::shared_ptr<Util::Text> m_EnergyTextPtr = nullptr;

    struct SunEnergy {
        std::shared_ptr<Util::GameObject> object;
        glm::vec2 velocity;
        bool collecting = false;
        float stopY = LevelManagerConfig::SUN_STOPPING_HEIGHT; // 隨機停止高度
    };

    std::vector<SunEnergy> m_SunEnergies;
    struct BeanProjectile {
        std::shared_ptr<Util::GameObject> object;
        int row = 0;
        float speed = LevelManagerConfig::BEAN_SPEED;
        float damage = LevelManagerConfig::BEAN_DAMAGE;
    };
    std::vector<BeanProjectile> m_BeanProjectiles;
    float m_SunSpawnTimer = 0.0f; // 計時器（秒）
    std::vector<std::shared_ptr<Zombie>> m_Zombies;
    float m_ZombieSpawnTimer = 0.0f;

    // 開場動畫控制
    bool m_IntroDone = false;

    // Wave system - using levels.json configuration
    std::vector<LevelConfig> m_AllLevelConfigs;
    LevelConfig m_CurrentLevelConfig;
    GameStateManager m_GameStateManager;
    float m_WaveStartTimer = 0.0f;  // Track time within current wave
    float m_WaveSpawnTimer = 0.0f;  // Timer for individual zombie spawns
    int m_ZombiesSpawnedInWave = 0;  // Count of zombies spawned in current wave
    int m_CurrentZombieSpawnTypeIndex = 0;  // Which zombie type in wave we're spawning
    float m_CurrentZombieTypeSpawnTimer = 0.0f;  // Timer for current zombie type
    int m_CurrentZombieTypeSpawned = 0;  // How many of current type spawned
    float m_ElapsedTime = 0.0f;  // Total elapsed time for win conditions
    int m_EnergyCollected = 0;  // Track collected energy for win condition

    // Level transition timers
    float m_WinDelayTimer = 0.0f;  // Delay before advancing to next level
    float m_LoseDelayTimer = 0.0f;  // Delay before restarting level
};

#endif