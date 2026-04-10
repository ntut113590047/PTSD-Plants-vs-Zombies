#include "LevelManager.hpp"
#include "Util/Input.hpp"
#include "Util/Text.hpp"
#include "Util/Animation.hpp"
#include "Util/Time.hpp"
#include "PlantRegistry.hpp"
#include "PeashooterPlant.hpp"
#include "SunflowerPlant.hpp"
#include "WallnutPlant.hpp"
#include "PotatoMinePlant.hpp"
#include "RepeaterPlant.hpp"
#include "CherryBombPlant.hpp"
#include "SnowPeaPlant.hpp"
#include "ChomperPlant.hpp"
#include "BasicZombie.hpp"
#include "ConeZombie.hpp"
#include "BucketZombie.hpp"
#include "PoleVaulterZombie.hpp"
#include "FlagZombie.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <unordered_set>

namespace fs = std::filesystem;

// Helper function to auto-detect frame count in a directory
static int GetFrameCountForPath(const std::string& basePath) {
    int maxFrame = 0;
    try {
        for (int i = 1; i <= 999; ++i) {
            std::string framePath = basePath + "/frame_" + std::to_string(i) + ".png";
            if (fs::exists(framePath)) {
                maxFrame = i;
            } else if (i > maxFrame + 10) {
                // Stop searching after 10 consecutive misses
                break;
            }
        }
    } catch (...) {
        // If filesystem check fails, return 0
    }
    return (maxFrame > 0) ? maxFrame : 8; // Default to 8 if nothing found
}

// Helper function to generate frame paths for any zombie/plant type
static std::vector<std::string> GetFramePaths(const std::string& resourcePath, int frameCount) {
    std::vector<std::string> paths;
    for (int i = 1; i <= frameCount; ++i) {
        paths.push_back(resourcePath + "/frame_" + std::to_string(i) + ".png");
    }
    return paths;
}


LevelManager::LevelManager(int level)
    : m_CurrentLevel(level) {}

void LevelManager::TriggerGameOver(Util::Renderer& root) {
    if (m_IsGameOver) {
        return;
    }

    m_IsGameOver = true;
    m_GameOverPanelShown = false;
    m_GameOverScaleTimer = 0.0f;
    m_GameOverShakeTimer = 0.0f;
    m_GameOverBasePosition = {0.0f, 0.0f};

    if (m_GameOverBoard) {
        root.RemoveChild(m_GameOverBoard);
        m_GameOverBoard = nullptr;
    }
    if (m_GameOverButton) {
        root.RemoveChild(m_GameOverButton);
        m_GameOverButton = nullptr;
    }

    m_GameOverWord = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/gameover_Word.png"),
        31.0f
    );
    m_GameOverWord->m_Transform.translation = m_GameOverBasePosition;
    m_GameOverWord->m_Transform.scale = {0.35f, 0.35f};
    root.AddChild(m_GameOverWord);
}

void LevelManager::UpdateGameOverAnimation(Util::Renderer& root, float deltaTime) {
    if (!m_GameOverWord) {
        return;
    }

    if (m_GameOverPanelShown && m_GameOverButton) {
        const auto mousePos = Util::Input::GetCursorPosition();
        m_GameOverButton->UpdateHoverState(mousePos.x, mousePos.y);
        if (m_GameOverButton->IsClicked()) {
            ChangeLevel(m_CurrentLevel, root);
            return;
        }

        return;
    }

    constexpr float kStartScale = 0.35f;
    constexpr float kMaxScale = 1.25f;
    constexpr float kScaleDuration = 1.10f;

    m_GameOverScaleTimer += deltaTime;
    const float t = std::clamp(m_GameOverScaleTimer / kScaleDuration, 0.0f, 1.0f);
    const float easeOut = 1.0f - (1.0f - t) * (1.0f - t);
    const float currentScale = kStartScale + (kMaxScale - kStartScale) * easeOut;
    m_GameOverWord->m_Transform.scale = {currentScale, currentScale};

    if (t >= 1.0f) {
        m_GameOverShakeTimer += deltaTime;
        const float shakeX = std::sin(m_GameOverShakeTimer * 24.0f) * 3.0f;
        const float shakeY = std::cos(m_GameOverShakeTimer * 20.0f) * 2.0f;
        m_GameOverWord->m_Transform.translation = {
            m_GameOverBasePosition.x + shakeX,
            m_GameOverBasePosition.y + shakeY
        };

        if (!m_GameOverPanelShown && m_GameOverShakeTimer >= 3.0f) {
            m_GameOverPanelShown = true;

            m_GameOverWord->SetVisible(false);

            m_GameOverBoard = std::make_shared<Util::GameObject>(
                std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/gameoverBoard.png"),
                32.0f
            );
            m_GameOverBoard->m_Transform.translation = {0.0f, 0.0f};
            root.AddChild(m_GameOverBoard);

            m_GameOverButton = std::make_shared<Button>(
                RESOURCE_DIR"/Image/Other/gameoverButton.png",
                RESOURCE_DIR"/Image/Other/gameoverButton_2.png"
            );
            m_GameOverButton->SetZIndex(33.0f);
            m_GameOverButton->SetPosition(0.0f, -120.0f);
            m_GameOverButton->SetScale(0.4f, 0.4f);
            root.AddChild(m_GameOverButton);
        }
    } else {
        m_GameOverWord->m_Transform.translation = m_GameOverBasePosition;
    }
}

std::string LevelManager::GetRewardPlantNameForCurrentLevel() const {
    if (m_CurrentLevel < 1 || m_CurrentLevel > static_cast<int>(m_AllLevelConfigs.size())) {
        return "sunflower";
    }

    const LevelConfig& current = m_AllLevelConfigs[m_CurrentLevel - 1];
    if (m_CurrentLevel < static_cast<int>(m_AllLevelConfigs.size())) {
        const LevelConfig& next = m_AllLevelConfigs[m_CurrentLevel];

        std::unordered_set<std::string> currentSet(current.available_plants.begin(), current.available_plants.end());
        for (const auto& plant : next.available_plants) {
            if (currentSet.find(plant) == currentSet.end()) {
                return plant;
            }
        }
    }

    if (!current.available_plants.empty()) {
        return current.available_plants.back();
    }
    return "sunflower";
}

void LevelManager::StartLevelClearReward(Util::Renderer& root) {
    if (m_IsLevelClearRewardActive || m_IsGameOver) {
        return;
    }

    m_IsLevelClearRewardActive = true;
    m_IsRewardCardLanded = false;
    m_IsRewardPanelShown = false;
    m_RewardClaimed = false;

    m_RewardPlantName = GetRewardPlantNameForCurrentLevel();
    PlantData rewardData = PlantRegistry::GetInstance().GetPlantData(m_RewardPlantName);
    const std::string cardPath = rewardData.cardImagePath.empty()
        ? std::string(RESOURCE_DIR "/Image/Plants/sunflowerCard.png")
        : rewardData.cardImagePath;

    m_DroppedRewardCard = std::make_shared<Button>(cardPath, cardPath);
    m_DroppedRewardCard->SetZIndex(36.0f);
    m_DroppedRewardCard->SetScale(0.75f, 0.75f);
    m_DroppedRewardCard->SetPosition(m_LastZombieDeathPosition.x, m_LastZombieDeathPosition.y + 12.0f);
    root.AddChild(m_DroppedRewardCard);

    m_RewardDropVelocity = {0.0f, 260.0f};
}

void LevelManager::UpdateLevelClearReward(Util::Renderer& root, float deltaTime) {
    if (!m_IsLevelClearRewardActive) {
        return;
    }

    if (!m_RewardClaimed) {
        if (!m_DroppedRewardCard) {
            return;
        }

        const float landingY = m_LastZombieDeathPosition.y;
        if (!m_IsRewardCardLanded) {
            glm::vec2 pos = m_DroppedRewardCard->m_Transform.translation;
            m_RewardDropVelocity.y += m_RewardDropGravity * deltaTime;
            pos += m_RewardDropVelocity * deltaTime;

            if (pos.y <= landingY) {
                pos.y = landingY;
                m_IsRewardCardLanded = true;
                m_RewardDropVelocity = {0.0f, 0.0f};
            }

            m_DroppedRewardCard->SetPosition(pos.x, pos.y);
            return;
        }

        const auto mousePos = Util::Input::GetCursorPosition();
        m_DroppedRewardCard->UpdateHoverState(mousePos.x, mousePos.y);

        if (!m_DroppedRewardCard->IsClicked()) {
            return;
        }

        m_RewardClaimed = true;
        root.RemoveChild(m_DroppedRewardCard);
        m_DroppedRewardCard = nullptr;

        m_GetPlantBoard = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR "/Image/Other/getPlantBoard.png"),
            34.0f
        );
        m_GetPlantBoard->m_Transform.translation = {0.0f, 0.0f};
        m_GetPlantBoard->m_Transform.scale = {0.9f, 0.9f};
        root.AddChild(m_GetPlantBoard);

        PlantData rewardData = PlantRegistry::GetInstance().GetPlantData(m_RewardPlantName);
        const std::string cardPath = rewardData.cardImagePath.empty()
            ? std::string(RESOURCE_DIR "/Image/Plants/sunflowerCard.png")
            : rewardData.cardImagePath;

        m_RewardCardDisplay = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(cardPath),
            35.0f
        );
        m_RewardCardDisplay->m_Transform.translation = {0.0f, 95.0f};
        m_RewardCardDisplay->m_Transform.scale = {0.9f, 0.9f};
        root.AddChild(m_RewardCardDisplay);

        m_NextLevelButton = std::make_shared<Button>(
            RESOURCE_DIR "/Image/Other/nextlevelButton.png",
            RESOURCE_DIR "/Image/Other/nextlevelButton_2.png"
        );
        m_NextLevelButton->SetZIndex(35.0f);
        m_NextLevelButton->SetPosition(0.0f, -210.0f);
        m_NextLevelButton->SetScale(0.25f, 0.25f);
        root.AddChild(m_NextLevelButton);

        m_IsRewardPanelShown = true;
        return;
    }

    if (m_IsRewardPanelShown && m_NextLevelButton) {
        const auto mousePos = Util::Input::GetCursorPosition();
        m_NextLevelButton->UpdateHoverState(mousePos.x, mousePos.y);
        if (m_NextLevelButton->IsClicked()) {
            if (m_CurrentLevel < 10) {
                ChangeLevel(m_CurrentLevel + 1, root);
            } else {
                ChangeLevel(1, root);
            }
        }
    }
}

std::shared_ptr<Zombie> LevelManager::SpawnZombie(const std::string& zombieType, int row, float y) {
    (void)y;

    if (zombieType == "ConeZombie") {
        std::string idlePath = RESOURCE_DIR"/Image/zombies/coneZombie";
        std::string attackPath = RESOURCE_DIR"/Image/zombies/coneZombieAttack";
        int idleFrames = GetFrameCountForPath(idlePath);
        int attackFrames = GetFrameCountForPath(attackPath);
        // Use minimum frame count to avoid loading non-existent files
        int frameCount = std::min(idleFrames, attackFrames);
        auto paths = GetFramePaths(idlePath, frameCount);
        return std::make_shared<ConeZombie>(paths, row, 35.0f);
    } else if (zombieType == "BucketZombie") {
        std::string idlePath = RESOURCE_DIR"/Image/zombies/bucketZombie";
        std::string attackPath = RESOURCE_DIR"/Image/zombies/bucketZombieAttack";
        int idleFrames = GetFrameCountForPath(idlePath);
        int attackFrames = GetFrameCountForPath(attackPath);
        // Use minimum frame count to avoid loading non-existent files
        int frameCount = std::min(idleFrames, attackFrames);
        auto paths = GetFramePaths(idlePath, frameCount);
        return std::make_shared<BucketZombie>(paths, row, 35.0f);
    } else if (zombieType == "PoleVaulterZombie") {
        std::string basePath = RESOURCE_DIR"/Image/zombies/polevaulter";
        int frameCount = GetFrameCountForPath(basePath);
        auto paths = GetFramePaths(basePath, frameCount);
        return std::make_shared<PoleVaulterZombie>(paths, row, 35.0f);
    } else if (zombieType == "FlagZombie") {
        std::string idlePath = RESOURCE_DIR"/Image/zombies/flagZombie";
        std::string attackPath = RESOURCE_DIR"/Image/zombies/flagZombieAttack";
        int idleFrames = GetFrameCountForPath(idlePath);
        int attackFrames = GetFrameCountForPath(attackPath);
        // Use minimum frame count to avoid loading non-existent files
        int frameCount = std::min(idleFrames, attackFrames);
        auto paths = GetFramePaths(idlePath, frameCount);
        return std::make_shared<FlagZombie>(paths, row, 35.0f);
    } else {
        // Default to BasicZombie
        std::string basePath = RESOURCE_DIR"/Image/zombies/zombie";
        int frameCount = GetFrameCountForPath(basePath);
        auto paths = GetFramePaths(basePath, frameCount);
        return std::make_shared<BasicZombie>(paths, row, 23.0f);
    }
}

bool LevelManager::IsGameLevel() const {
    return m_CurrentLevel != 0;
}

void LevelManager::LoadLevel(Util::Renderer& root) {

    // ===== 韏瑕??恍 =====
    if (m_CurrentLevel == 0) {
        m_Background = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Background/startBackGround.png"),
            -10
        );
        root.AddChild(m_Background);

        auto startButton = std::make_shared<Button>(
            RESOURCE_DIR"/Image/Other/startButton.png",
            RESOURCE_DIR"/Image/Other/startButton_2.png"
        );
        startButton->SetZIndex(5);
        startButton->SetScale(0.7f, 0.7f);
        startButton->SetPosition(260.0f, 210.0f);
        root.AddChild(startButton);
        m_Buttons.push_back(startButton);

        return; // 韏瑕??恍銝?憪??∠???摮???
    }

    // ===== 甇??? =====
    // Load level configuration from JSON
    if (m_AllLevelConfigs.empty()) {
        m_AllLevelConfigs = LevelConfigParser::LoadFromFile(RESOURCE_DIR"/levels/levels.json");
    }

    // Get config for current level (1-indexed)
    if (m_CurrentLevel >= 1 && m_CurrentLevel <= static_cast<int>(m_AllLevelConfigs.size())) {
        m_CurrentLevelConfig = m_AllLevelConfigs[m_CurrentLevel - 1];
    } else {
        // Fallback if level not found
        m_CurrentLevelConfig.level_number = m_CurrentLevel;
        m_CurrentLevelConfig.background = "phase3.png";
        m_CurrentLevelConfig.starting_energy = 50;
    }

    // Reset wave counters
    m_GameStateManager.Reset();
    m_GameStateManager.SetTotalWaves(m_CurrentLevelConfig.waves.size());
    m_GameStateManager.SetWinCondition(m_CurrentLevelConfig.win_condition);

    m_WaveStartTimer = 0.0f;
    m_WaveSpawnTimer = 0.0f;
    m_ZombiesSpawnedInWave = 0;
    m_CurrentZombieSpawnTypeIndex = 0;
    m_CurrentZombieTypeSpawnTimer = 0.0f;
    m_CurrentZombieTypeSpawned = 0;
    m_ElapsedTime = 0.0f;
    m_EnergyCollected = 0;

    std::string path = RESOURCE_DIR"/Image/Background/" + m_CurrentLevelConfig.background;
    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(path), -10
    );
	m_Background->m_Transform.scale = {1.1f, 1.1f};
    m_Background->m_Transform.translation = {200.0f, 0.0f};
    root.AddChild(m_Background);

    // ===== ????摮???=====
    m_WordPhase = 1;
    m_WordTimer = 0.0f;
    m_Word = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/word1.png"),
        10
    );
    m_Word->m_Transform.scale = {0.2f, 0.2f}; // ??蝮格
    m_Word->m_Transform.translation = {0.0f, 0.0f}; // ??雿蔭
    root.AddChild(m_Word);

    // ===== ????局 =====
    m_CardSlot = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/cardplot.png"),
        10
    );
    m_CardSlot->m_Transform.scale = {0.25f, 0.25f};
    m_CardSlot->m_Transform.translation = {-300.0f, m_CardSlotTargetY};
    root.AddChild(m_CardSlot);
    m_CardSlotActive = true;

    // ===== ?????蝵?=====
    m_CardPositions.clear();
    float startX = m_CardSlot->m_Transform.translation.x - 150.0f;
    float startY = m_CardSlot->m_Transform.translation.y;
    float spacing = 80.0f;
    for (size_t i = 0; i < LevelManagerConfig::NUM_CARDS; ++i) {
        m_CardPositions.push_back(glm::vec2(startX + i * spacing, startY));
    }

    // ===== ???????=====
    auto& registry = PlantRegistry::GetInstance();
    m_LevelPlants.clear();
    for (const auto& plantName : m_CurrentLevelConfig.available_plants) {
        m_LevelPlants.push_back(registry.GetPlantData(plantName));
    }
    // ===== ?萄遣?∠? =====
    m_Cards.clear();
    m_CardVisuals.clear();
    for (size_t i = 0; i < LevelManagerConfig::NUM_CARDS; ++i) {
        if (i >= m_LevelPlants.size()) break;
        auto& data = m_LevelPlants[i];
        auto card = std::make_shared<PlantCard>(data, m_CardPositions[i].x, m_CardPositions[i].y);
        card->SetZIndex(m_CardSlot->GetZIndex() + 1);
		card->m_Transform.scale = {0.6f, 0.6f};
        root.AddChild(card);
        m_Cards.push_back(card);

        // ?∠?銝?拙惜?桃蔗嚗??頞喳凝??餉????勗?敺銝?
        auto energyDim = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/card_dim_overlay.png"),
            card->GetZIndex() + 0.1f
        );
        energyDim->m_Transform.translation = card->m_Transform.translation;
        auto cardSize = card->GetScaledSize();
        energyDim->m_Transform.scale = {cardSize.x, cardSize.y};
        energyDim->SetVisible(false);
        root.AddChild(energyDim);

        auto cooldownDim = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/card_cooldown_overlay.png"),
            card->GetZIndex() + 0.2f
        );
        cooldownDim->m_Transform.translation = card->m_Transform.translation;
        cooldownDim->m_Transform.scale = {cardSize.x, cardSize.y};
        cooldownDim->SetVisible(false);
        root.AddChild(cooldownDim);

        m_CardVisuals.push_back({energyDim, cooldownDim});
    }

    // ===== ?????芰雯??=====
    m_GrassGrid.assign(5, std::vector<bool>(9, false)); // 5銵???false銵函內蝛?
    m_RowAllowed.assign(5, false);
    for (int row : m_CurrentLevelConfig.rows_unlocked) {
        if (row >= 0 && row < 5) {
            m_RowAllowed[row] = true;
        }
    }

    // ?寞??迂???詨???蝞雯?澆?璅?
    int allowedRowCount = 0;
    int minRow = 4, maxRow = 0;
    for (int r = 0; r < 5; ++r) {
        if (m_RowAllowed[r]) {
            allowedRowCount++;
            minRow = std::min(minRow, r);
            maxRow = std::max(maxRow, r);
        }
    }

    m_GridMinRow = minRow;
    m_GridMaxRow = maxRow;

    if (allowedRowCount > 0) {
        float baseTop = 200.0f;
        float baseBottom = -270.0f;
        float totalHeight = baseTop - baseBottom; // 470
        float baselineSpacing = totalHeight / (5 - 1); // ?箇?銵頝?117.5

        if (allowedRowCount == 1) {
            // ?株?撅葉嚗蝙?典蝖?渲?
            m_GridCellHeight = baselineSpacing;
            m_GridTopY = baseTop - minRow * baselineSpacing;
            m_GridBottomY = m_GridTopY;
        } else {
            // 憭????
            m_GridCellHeight = baselineSpacing;
            m_GridTopY = baseTop - minRow * baselineSpacing;
            m_GridBottomY = baseTop - maxRow * baselineSpacing;
        }
    }

    // ===== ????銵?? =====
    m_LawnMowers.clear();
    {
        float leftX = m_GridLeftX;

        for (int r = 0; r < 5; ++r) {
            if (r >= static_cast<int>(m_RowAllowed.size()) || !m_RowAllowed[r]) {
                continue;
            }
            float rowY = m_GridTopY - (r - m_GridMinRow) * m_GridCellHeight;
            auto mower = std::make_shared<LawnMower>(r);
            mower->m_Transform.translation = {leftX - 85.0f, rowY};
            root.AddChild(mower);
            m_LawnMowers.push_back(mower);
        }
    }

    m_PlacedPlants.clear();
    m_SelectedCard = nullptr;
    m_FollowingPlant = nullptr;
    m_PreviewPlant = nullptr;
    m_PlayerEnergy = m_CurrentLevelConfig.starting_energy;

    m_SunEnergies.clear();
    m_BeanProjectiles.clear();
    m_SunSpawnTimer = 0.0f;
    m_Zombies.clear();
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // ===== ????＊蝷箸?摮?=====
    m_EnergyTextPtr = std::make_shared<Util::Text>("C:/PTSD-Plants-vs-Zombies/PTSD/assets/fonts/Inter.ttf", 24, std::to_string(m_PlayerEnergy), Util::Color(0, 0, 0));
    m_EnergyText = std::make_shared<Util::GameObject>(m_EnergyTextPtr, 30);
    m_EnergyText->m_Transform.translation = {-540.0f, 285.0f}; // 撌虫?閫?蝵殷??ㄐ靽格xy摨扳?
    root.AddChild(m_EnergyText);

    m_IntroDone = true; // ??????
}

void LevelManager::Update(Util::Renderer& root, float deltaTime) {
    // ===== 韏瑕??恍?? =====
    if (m_CurrentLevel == 0) {
        auto mousePos = Util::Input::GetCursorPosition();
        for (auto& btn : m_Buttons) {
            btn->UpdateHoverState(mousePos.x, mousePos.y);
            if (btn->IsClicked()) {
                ChangeLevel(1, root); // 暺?憪??脣甇???
                return;
            }
        }
        return; // 韏瑕??恍?芾?????銝????????
    }

    // ===== 甇?????? =====
    if (!m_IntroDone || !IsGameLevel()) return;

    if (m_IsGameOver) {
        UpdateGameOverAnimation(root, deltaTime);
        return;
    }

    if (m_IsLevelClearRewardActive) {
        UpdateLevelClearReward(root, deltaTime);
        return;
    }

    if (m_WordPhase > 0 && m_Word) {
        m_WordTimer += deltaTime; // 蝝航???嚗?嚗?
        auto LerpScale = [](float a, float b, float t) { return a + (b - a) * t; };

        switch (m_WordPhase) {
            case 1: {
                float t = std::min(m_WordTimer / 0.5f, 1.0f); // 0.5蝘???
                m_Word->m_Transform.scale = { LerpScale(0.2f,0.3f,t), LerpScale(0.2f,0.3f,t) };
                if (m_WordTimer > 0.5f) {
                    root.RemoveChild(m_Word);
                    m_Word = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/word2.png"), 10
                    );
                    m_Word->m_Transform.scale = {0.2f, 0.2f}; // 閮剔蔭??蝮格
                    m_Word->m_Transform.translation = {0.0f, 0.0f}; // 閮剔蔭??雿蔭
                    root.AddChild(m_Word);
                    m_WordPhase = 2; m_WordTimer = 0.0f;
                }
                break;
            }
            case 2: {
                float t = std::min(m_WordTimer / 0.5f, 1.0f); // 0.5蝘???
                m_Word->m_Transform.scale = { LerpScale(0.2f,0.3f,t), LerpScale(0.2f,0.3f,t) };
                if (m_WordTimer > 0.5f) {
                    root.RemoveChild(m_Word);
                    m_Word = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/word3.png"), 10
                    );
                    m_Word->m_Transform.scale = {0.7f,0.7f};
                    m_Word->m_Transform.translation = {0.0f, 0.0f}; // 閮剔蔭??雿蔭
                    root.AddChild(m_Word);
                    m_WordPhase = 3; m_WordTimer = 0.0f;
                }
                break;
            }
            case 3:
                if (m_WordTimer > 1.17f) { // 蝝?.17蝘＊蝷?
                    root.RemoveChild(m_Word);
                    m_Word = nullptr;
                    m_WordPhase = 0;
                }
                break;
        }
    }

    // ===== ???嚗蝵格???=====
    if (m_WordPhase == 0) {
        // Track elapsed time and update win condition checks
        m_ElapsedTime += deltaTime;

        // ?湔?賡?憿舐內
        if (m_EnergyTextPtr) {
            m_EnergyTextPtr->SetText(std::to_string(m_PlayerEnergy));
        }

        // ?賡???Ｙ? (瘥?蝘璈???
        m_SunSpawnTimer += deltaTime;
        const float spawnInterval = 5.0f;
        if (m_SunSpawnTimer >= spawnInterval) {
            m_SunSpawnTimer = 0.0f;
            float minX = -540.0f;
            float maxX = 540.0f;
            float x = minX + static_cast<float>(std::rand()) / RAND_MAX * (maxX - minX);
            float y = 330.0f;

            std::vector<std::string> energyPaths;
            for (int i = 1; i <= 29; ++i) {
                energyPaths.push_back(RESOURCE_DIR"/Image/Other/energy/frame_" + std::to_string(i) + ".png");
            }

            auto energyObj = std::make_shared<Util::GameObject>(
                std::make_shared<Util::Animation>(energyPaths, true, 50, true, 0), 30
            );
            energyObj->m_Transform.translation = {x, y};
            energyObj->m_Transform.scale = {1.0f, 1.0f};
            root.AddChild(energyObj);

            float dropSpeed = -40.0f;
            float stopY = -120.0f - static_cast<float>(std::rand()) / RAND_MAX * 80.0f;
            m_SunEnergies.push_back({energyObj, {0.0f, dropSpeed}, false, stopY, 0.0f});
        }

        // ?湔?∠??瑕??函???
        for (size_t i = 0; i < m_Cards.size(); ++i) {
            auto& card = m_Cards[i];
            card->UpdateCooldown(deltaTime); // 雿輻?祕??deltaTime
            bool hasEnergy = (m_PlayerEnergy >= card->GetData().cost);
            // 銝?函??∠?霈, ??刻??迤撣訾漁摨?
            card->SetEnergyAvailable(hasEnergy && card->IsReady());

            if (i < m_CardVisuals.size()) {
                auto& visuals = m_CardVisuals[i];
                auto cardSize = card->GetScaledSize();

                if (visuals.energyDimOverlay) {
                    visuals.energyDimOverlay->m_Transform.translation = card->m_Transform.translation;
                    visuals.energyDimOverlay->m_Transform.scale = {cardSize.x, cardSize.y};
                    visuals.energyDimOverlay->SetVisible(!hasEnergy);
                }

                if (visuals.cooldownOverlay) {
                    float remainRatio = 0.0f;
                    if (card->GetData().cooldown > 0.0f && card->IsCoolingDown()) {
                        remainRatio = std::clamp(1.0f - card->GetCooldownProgress(), 0.0f, 1.0f);
                    }

                    if (remainRatio > 0.0f) {
                        float h = cardSize.y * remainRatio;
                        visuals.cooldownOverlay->m_Transform.scale = {cardSize.x, h};
                        visuals.cooldownOverlay->m_Transform.translation = {
                            card->m_Transform.translation.x,
                            card->m_Transform.translation.y + cardSize.y * 0.5f - h * 0.5f
                        };
                        visuals.cooldownOverlay->SetVisible(true);
                    } else {
                        visuals.cooldownOverlay->SetVisible(false);
                    }
                }
            }
        }

        // Wave-based zombie spawning system
        if (m_CurrentLevel >= 1 && m_CurrentLevel <= 10 && m_GameStateManager.GetCurrentWave() < static_cast<int>(m_CurrentLevelConfig.waves.size())) {
            Wave& currentWave = m_CurrentLevelConfig.waves[m_GameStateManager.GetCurrentWave()];
            m_WaveStartTimer += deltaTime;

            // Check if wave delay has passed
            if (m_WaveStartTimer >= currentWave.start_delay) {
                // Ensure FlagZombie leads the wave when present.
                if (m_CurrentZombieSpawnTypeIndex == 0 && m_CurrentZombieTypeSpawned == 0) {
                    auto it = std::find_if(
                        currentWave.zombies.begin(),
                        currentWave.zombies.end(),
                        [](const ZombieSpawn& z) {
                            return z.type == "FlagZombie" && z.count > 0;
                        }
                    );
                    if (it != currentWave.zombies.end() && it != currentWave.zombies.begin()) {
                        ZombieSpawn flagSpawn = *it;
                        currentWave.zombies.erase(it);
                        currentWave.zombies.insert(currentWave.zombies.begin(), flagSpawn);
                    }
                }

                // Wave is active, spawn zombies
                if (m_CurrentZombieSpawnTypeIndex < static_cast<int>(currentWave.zombies.size())) {
                    const ZombieSpawn& spawnConfig = currentWave.zombies[m_CurrentZombieSpawnTypeIndex];

                    // Check if we've spawned all of this zombie type
                    if (m_CurrentZombieTypeSpawned < spawnConfig.count) {
                        m_CurrentZombieTypeSpawnTimer += deltaTime;

                        // Spawn zombie if timer exceeds interval, accounting for delay_offset on first spawn
                        float spawnThreshold = spawnConfig.spawn_interval;
                        if (m_CurrentZombieTypeSpawned == 0) {
                            spawnThreshold = spawnConfig.delay_offset + spawnConfig.spawn_interval;
                        }

                        if (m_CurrentZombieTypeSpawnTimer >= spawnThreshold) {
                            // Get a random allowed row
                            int spawnRow = 2; // Default to middle row
                            std::vector<int> allowedRows;
                            for (int r = 0; r < 5; ++r) {
                                if (r < static_cast<int>(m_RowAllowed.size()) && m_RowAllowed[r]) {
                                    allowedRows.push_back(r);
                                }
                            }
                            if (!allowedRows.empty()) {
                                spawnRow = allowedRows[std::rand() % allowedRows.size()];
                            }

                            float topY = 200.0f;
                            float bottomY = -270.0f;
                            int rows = 5;
                            float cellHeight = (topY - bottomY) / (rows - 1);
                            float spawnY = topY - spawnRow * cellHeight + 20.0f;
                            float spawnX = 680.0f; // Right side of screen

                            auto zombie = SpawnZombie(spawnConfig.type, spawnRow, spawnY);
                            zombie->m_Transform.translation = {spawnX, spawnY};
                            root.AddChild(zombie);
                            m_Zombies.push_back(zombie);
                            m_GameStateManager.RegisterZombieSpawned();

                            m_CurrentZombieTypeSpawned++;
                            m_ZombiesSpawnedInWave++;
                            m_CurrentZombieTypeSpawnTimer -= spawnThreshold; // Subtract to handle timing properly for next spawn
                        }
                    } else {
                        // Move to next zombie type in this wave
                        m_CurrentZombieSpawnTypeIndex++;
                        m_CurrentZombieTypeSpawned = 0;
                        m_CurrentZombieTypeSpawnTimer = 0.0f;
                    }
                } else {
                    // All zombies for this wave have been scheduled.
                    // Move to next wave immediately (no need to clear previous wave first).
                    m_GameStateManager.AdvanceWave();
                    m_WaveStartTimer = 0.0f;
                    m_ZombiesSpawnedInWave = 0;
                    m_CurrentZombieSpawnTypeIndex = 0;
                    m_CurrentZombieTypeSpawned = 0;
                    m_CurrentZombieTypeSpawnTimer = 0.0f;
                }
            }
        }

        // Check win/lose conditions periodically
        if (!m_IsLevelClearRewardActive && m_GameStateManager.CheckWinCondition(m_ElapsedTime, m_EnergyCollected)) {
            StartLevelClearReward(root);
            return;
        }
        if (m_GameStateManager.HasLost()) {
            TriggerGameOver(root);
            return;
        }

        // ?圈洛嚗漱?望??拚??亙皜祈??餅?嚗?鞊????刻撌梁? Attack ?抒撠?撠嚗?
        for (auto& plant : m_PlacedPlants) {
            plant->Update(deltaTime);

            if (auto sunflower = std::dynamic_pointer_cast<SunflowerPlant>(plant); sunflower && sunflower->TryProduceSun()) {
                const int producedSun = sunflower->GetSunProductionAmount();
                m_PlayerEnergy += producedSun;
                m_EnergyCollected += producedSun;

                // Create a reward-drop-style energy orb
                std::vector<std::string> energyPaths;
                for (int i = 1; i <= 29; ++i) {
                    energyPaths.push_back(RESOURCE_DIR"/Image/Other/energy/frame_" + std::to_string(i) + ".png");
                }

                auto energyObj = std::make_shared<Util::GameObject>(
                    std::make_shared<Util::Animation>(energyPaths, true, 50, true, 0), 30
                );
                energyObj->m_Transform.translation = plant->m_Transform.translation;
                energyObj->m_Transform.scale = {1.0f, 1.0f};
                root.AddChild(energyObj);

                float initialUpVelocity = 260.0f;
                glm::vec2 initialVelocity = {0.0f, initialUpVelocity};
                float gravity = -520.0f;
                m_SunEnergies.push_back({energyObj, initialVelocity, false, plant->m_Transform.translation.y, gravity});
            }

            auto projectile = plant->Attack(m_Zombies, deltaTime);
            if (projectile.has_value() && projectile->object) {
                root.AddChild(projectile->object);
                m_BeanProjectiles.push_back({
                    projectile->object,
                    projectile->row,
                    projectile->speed,
                    projectile->damage,
                    projectile->slowMultiplier,
                    projectile->slowDuration
                });
            }
        }

        // ?湔鞊????抬?憌??１?宏??
        for (int i = static_cast<int>(m_BeanProjectiles.size()) - 1; i >= 0; --i) {
            auto& bean = m_BeanProjectiles[i];
            bean.object->m_Transform.translation.x += bean.speed * deltaTime;

            bool consumed = false;
            for (auto& zombie : m_Zombies) {
                if (zombie->GetRow() != bean.row) {
                    continue;
                }
                float dx = zombie->m_Transform.translation.x - bean.object->m_Transform.translation.x;
                float dy = std::abs(zombie->m_Transform.translation.y - bean.object->m_Transform.translation.y);
                if (dx <= 18.0f && dx >= -18.0f && dy <= 26.0f) {
                    zombie->TakeDamage(bean.damage);
                    if (bean.slowMultiplier < 1.0f && bean.slowDuration > 0.0f) {
                        zombie->ApplySlow(bean.slowMultiplier, bean.slowDuration);
                    }
                    consumed = true;
                    break;
                }
            }

            if (consumed || bean.object->m_Transform.translation.x > 780.0f) {
                root.RemoveChild(bean.object);
                m_BeanProjectiles.erase(m_BeanProjectiles.begin() + i);
            }
        }

        // ?脰?璈??菜葫畾剖??亥孛 ??瞈瘣颱蒂璈急?????悌撅?
        for (auto& mower : m_LawnMowers) {
            if (mower->IsActive()) {
                mower->m_Transform.translation.x += LawnMower::kMoveSpeed * deltaTime;
                for (auto& zombie : m_Zombies) {
                    if (zombie->GetRow() != mower->GetRow()) continue;
                    float dx = zombie->m_Transform.translation.x - mower->m_Transform.translation.x;
                    if (dx >= -40.0f && dx <= 40.0f) {
                        zombie->TakeDamage(99999.0f);
                    }
                }
            } else {
                for (auto& zombie : m_Zombies) {
                    if (zombie->GetRow() != mower->GetRow()) continue;
                    float dx = zombie->m_Transform.translation.x - mower->m_Transform.translation.x;
                    if (dx <= 35.0f) {
                        mower->Activate();
                        break;
                    }
                }
            }
        }
        // 蝘駁撌脤??箏?渡?Ｙ??脰?璈?
        for (int i = static_cast<int>(m_LawnMowers.size()) - 1; i >= 0; --i) {
            if (m_LawnMowers[i]->IsActive() &&
                m_LawnMowers[i]->m_Transform.translation.x > 800.0f) {
                root.RemoveChild(m_LawnMowers[i]);
                m_LawnMowers.erase(m_LawnMowers.begin() + i);
            }
        }

        // ?圈洛嚗悌撅閫豢??拇???銝行???血?????
        const float gameOverLineX = (m_GridLeftX - 85.0f) - 10.0f;
        for (auto& zombie : m_Zombies) {
            // Unified fail line: any zombie type crossing this line ends the game.
            if (zombie->m_Transform.translation.x <= gameOverLineX) {
                TriggerGameOver(root);
                return;
            }

            zombie->Tick(deltaTime);

            std::shared_ptr<Plant> blockingPlant = nullptr;
            float nearestDist = 1e9f;

            for (auto& plant : m_PlacedPlants) {
                if (plant->GetRow() != zombie->GetRow()) {
                    continue;
                }
                float dist = std::abs(zombie->m_Transform.translation.x - plant->m_Transform.translation.x);
                if (dist <= 55.0f && dist < nearestDist) {
                    nearestDist = dist;
                    blockingPlant = plant;
                }
            }

            if (blockingPlant) {
                if (auto poleVaulter = std::dynamic_pointer_cast<PoleVaulterZombie>(zombie); poleVaulter && !poleVaulter->HasVaulted()) {
                    poleVaulter->VaultOverPlant();
                    zombie->SetAttacking(false);
                    continue;
                }

                zombie->SetAttacking(true);
                if (zombie->TryAttack(deltaTime)) {
                    blockingPlant->TakeDamage(zombie->GetAttackDamage());
                }
            } else {
                zombie->SetAttacking(false);
                zombie->Update(deltaTime);
            }
        }

        // 蝘駁甇颱滿?悌撅?頞?畾剖?
        for (int i = static_cast<int>(m_Zombies.size()) - 1; i >= 0; --i) {
            auto& z = m_Zombies[i];
            if (z->IsDead()) {
                m_LastZombieDeathPosition = z->m_Transform.translation;
                m_GameStateManager.RegisterZombieDied();
                root.RemoveChild(z);
                m_Zombies.erase(m_Zombies.begin() + i);
            } else if (z->m_Transform.translation.x < -760.0f) {
                TriggerGameOver(root);
                return;
            }
        }

        if (!m_IsLevelClearRewardActive && m_GameStateManager.CheckWinCondition(m_ElapsedTime, m_EnergyCollected)) {
            StartLevelClearReward(root);
            return;
        }

        // 蝘駁甇颱滿璊銝阡??暹摮?
        for (int i = static_cast<int>(m_PlacedPlants.size()) - 1; i >= 0; --i) {
            auto& p = m_PlacedPlants[i];
            if (p->IsDead()) {
                int row = p->GetRow();
                int col = p->GetCol();
                if (row >= 0 && row < static_cast<int>(m_GrassGrid.size()) &&
                    col >= 0 && col < static_cast<int>(m_GrassGrid[row].size())) {
                    m_GrassGrid[row][col] = false;
                }
                
                root.RemoveChild(p);
                m_PlacedPlants.erase(m_PlacedPlants.begin() + i);
            }
        }

        auto mousePos = Util::Input::GetCursorPosition();

        // ?湔?賡?????
        {
            glm::vec2 targetPos = {-540.0f, 300.0f}; // ?賡???銝銝暺?蝵?
            float deltaSeconds = deltaTime; // 雿輻?祕??deltaTime
            for (int i = static_cast<int>(m_SunEnergies.size()) - 1; i >= 0; --i) {
                auto& sun = m_SunEnergies[i];
                auto& obj = sun.object;
                glm::vec2 pos = obj->m_Transform.translation;

                if (sun.collecting) {
                    glm::vec2 diff = targetPos - pos;
                    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                    if (dist < 8.0f) {
                        root.RemoveChild(obj);
                        m_SunEnergies.erase(m_SunEnergies.begin() + i);
                        m_PlayerEnergy += 25;
                        m_EnergyCollected += 25;  // Track energy for win condition
                        continue;
                    }
                    glm::vec2 dir = diff / dist;
                    float collectSpeed = std::max(200.0f, std::min(1200.0f, dist * 6.0f));
                    obj->m_Transform.translation += dir * collectSpeed * deltaSeconds; // 敺翰?唳
                } else {
                    // Apply gravity to velocity
                    sun.velocity.y += sun.gravity * deltaSeconds;

                    obj->m_Transform.translation += sun.velocity * deltaSeconds;

                    if (obj->m_Transform.translation.y < sun.stopY) {
                        obj->m_Transform.translation.y = sun.stopY;
                        sun.velocity = {0.0f, 0.0f};
                    }

                    // 憒?憌摨撠勗?歹?靽嚗?
                    if (obj->m_Transform.translation.y < -330.0f) {
                        root.RemoveChild(obj);
                        m_SunEnergies.erase(m_SunEnergies.begin() + i);
                        continue;
                    }
                    float mouseDist = std::sqrt((mousePos.x - pos.x) * (mousePos.x - pos.x) + (mousePos.y - pos.y) * (mousePos.y - pos.y));
                    if (mouseDist < 30.0f) {
                        sun.collecting = true;
                    }
                }
            }
        }


        // 瑼Ｘ?∠?暺?
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB) && !m_SelectedCard) {
            for (auto& card : m_Cards) {
                if (card->IsReady() && m_PlayerEnergy >= card->GetData().cost) {
                    // 閮?撖阡?憭批?
                    auto scaledSize = card->GetScaledSize();
                    float cardWidth = scaledSize.x;
                    float cardHeight = scaledSize.y;
                    float left = card->m_Transform.translation.x - cardWidth * 0.5f;
                    float right = card->m_Transform.translation.x + cardWidth * 0.5f;
                    float bottom = card->m_Transform.translation.y - cardHeight * 0.5f;
                    float top = card->m_Transform.translation.y + cardHeight * 0.5f;
                    if (mousePos.x >= left && mousePos.x <= right &&
                        mousePos.y >= bottom && mousePos.y <= top) {
                        m_SelectedCard = card;
                        m_FollowingPlant = std::make_shared<Util::GameObject>(
                            std::make_shared<Util::Animation>(card->GetData().plantAnimationPaths, true, 50, true, 0), 20
                        );
                        m_FollowingPlant->m_Transform.scale = {card->GetData().scale, card->GetData().scale};
                        root.AddChild(m_FollowingPlant);
                        break;
                    }
                }
            }
        }

        // ?湔頝璊雿蔭
        if (m_FollowingPlant) {
            m_FollowingPlant->m_Transform.translation = {mousePos.x, mousePos.y};

            // 瑼Ｘ?汗雿蔭
            int previewRow = -1, previewCol = -1;
            float cellWidth = (m_GridRightX - m_GridLeftX) / (m_GridCols - 1);

            for (int r = 0; r < 5; ++r) {
                if (!m_RowAllowed[r]) continue;

                // 閮?甇方??摨扳?嚗?潭?撠??撠?蝘鳴?
                float rowY = m_GridTopY - (r - m_GridMinRow) * m_GridCellHeight;

                for (int c = 0; c < m_GridCols; ++c) {
                    float cellX = m_GridLeftX + c * cellWidth;
                    float halfSizeX = cellWidth / 2.0f;
                    float halfSizeY = m_GridCellHeight / 2.0f;
                    if (mousePos.x >= cellX - halfSizeX && mousePos.x < cellX + halfSizeX &&
                        mousePos.y <= rowY + halfSizeY && mousePos.y > rowY - halfSizeY) {
                        previewRow = r;
                        previewCol = c;
                        break;
                    }
                }
                if (previewRow != -1) break;
            }

            // ?湔?汗璊
            if (previewRow != -1 && previewCol != -1 && m_RowAllowed[previewRow] && !m_GrassGrid[previewRow][previewCol]) {
                if (!m_PreviewPlant) {
                    // ?萄遣?汗璊嚗???嚗?
                    m_PreviewPlant = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Animation>(m_SelectedCard->GetData().plantAnimationPaths, true, 50, true, 0), 10
                    );
                    m_PreviewPlant->m_Transform.scale = {m_SelectedCard->GetData().scale, m_SelectedCard->GetData().scale};
                    root.AddChild(m_PreviewPlant);
                }
                float cellX = m_GridLeftX + previewCol * cellWidth;
                float rowY = m_GridTopY - (previewRow - m_GridMinRow) * m_GridCellHeight;
                m_PreviewPlant->m_Transform.translation = {cellX, rowY};
            } else {
                // 蝘駁?汗璊
                if (m_PreviewPlant) {
                    root.RemoveChild(m_PreviewPlant);
                    m_PreviewPlant = nullptr;
                }
            }
        } else {
            // 瘝?頝璊??閬???閬?
            if (m_PreviewPlant) {
                root.RemoveChild(m_PreviewPlant);
                m_PreviewPlant = nullptr;
            }
        }

        // 瑼Ｘ?曄蔭
        if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB) && m_SelectedCard) {
            // 憒???閬賣??拐?雿蔭??嚗?乩蝙?券?閬賭?蝵?
            if (m_PreviewPlant) {
                // ?曉撠??摮?璅?
                float cellWidth = (m_GridRightX - m_GridLeftX) / (m_GridCols - 1);
                glm::vec2 previewPos = m_PreviewPlant->m_Transform.translation;
                int row = -1, col = -1;

                for (int r = 0; r < 5; ++r) {
                    if (!m_RowAllowed[r]) continue;

                    float rowY = m_GridTopY - (r - m_GridMinRow) * m_GridCellHeight;
                    if (std::abs(previewPos.y - rowY) < 1.0f) {
                        row = r;
                        break;
                    }
                }

                for (int c = 0; c < m_GridCols; ++c) {
                    float cellX = m_GridLeftX + c * cellWidth;
                    if (std::abs(previewPos.x - cellX) < 1.0f) {
                        col = c;
                        break;
                    }
                }

                if (row != -1 && col != -1 && m_RowAllowed[row] && !m_GrassGrid[row][col]) {
                    // 靘?遣蝡????拚???
                    std::shared_ptr<Plant> placedPlant;
                    const auto& data = m_SelectedCard->GetData();
                    if (data.name == "bean" || data.name == "peashooter") {
                        placedPlant = std::make_shared<PeashooterPlant>(data);
                    } else if (data.name == "sunflower") {
                        placedPlant = std::make_shared<SunflowerPlant>(data);
                    } else if (data.name == "walnut") {
                        placedPlant = std::make_shared<WallnutPlant>(data);
                    } else if (data.name == "potato") {
                        placedPlant = std::make_shared<PotatoMinePlant>(data);
                    } else if (data.name == "repeater") {
                        placedPlant = std::make_shared<RepeaterPlant>(data);
                    } else if (data.name == "cherry") {
                        placedPlant = std::make_shared<CherryBombPlant>(data);
                    } else if (data.name == "snowpea") {
                        placedPlant = std::make_shared<SnowPeaPlant>(data);
                    } else if (data.name == "chomper") {
                        placedPlant = std::make_shared<ChomperPlant>(data);
                    } else {
                        placedPlant = std::make_shared<Plant>(
                            data,
                            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, true, 0),
                            20.0f
                        );
                    }

                    placedPlant->m_Transform.translation = m_PreviewPlant->m_Transform.translation;
                    placedPlant->SetGridPosition(row, col);
                    root.AddChild(placedPlant);
                    m_PlacedPlants.push_back(placedPlant);

                    root.RemoveChild(m_PreviewPlant);
                    m_GrassGrid[row][col] = true;
                    m_PlayerEnergy -= data.cost;
                    m_SelectedCard->StartCooldown();

                    // 皜?
                    m_PreviewPlant = nullptr;
                    root.RemoveChild(m_FollowingPlant);
                    m_FollowingPlant = nullptr;
                    m_SelectedCard = nullptr;
                }
            } else {
                // 瘝??汗璊嚗?瘨??
                root.RemoveChild(m_FollowingPlant);
                m_FollowingPlant = nullptr;
                m_SelectedCard = nullptr;
            }
        }
    }
}

void LevelManager::ChangeLevel(int level, Util::Renderer& root) {
    if (m_Background) { root.RemoveChild(m_Background); m_Background = nullptr; }
    if (m_CardSlot) { root.RemoveChild(m_CardSlot); m_CardSlot = nullptr; }
    if (m_Word) { root.RemoveChild(m_Word); m_Word = nullptr; }
    for (auto& btn : m_Buttons) root.RemoveChild(btn);
    for (auto& card : m_Cards) root.RemoveChild(card);
    for (auto& visuals : m_CardVisuals) {
        if (visuals.energyDimOverlay) root.RemoveChild(visuals.energyDimOverlay);
        if (visuals.cooldownOverlay) root.RemoveChild(visuals.cooldownOverlay);
    }
    for (auto& z : m_Zombies) {
        if (z) root.RemoveChild(z);
    }
    for (auto& mower : m_LawnMowers) root.RemoveChild(mower);
    for (auto& plant : m_PlacedPlants) root.RemoveChild(plant);
    for (auto& bean : m_BeanProjectiles) {
        if (bean.object) root.RemoveChild(bean.object);
    }
    for (auto& sun : m_SunEnergies) {
        if (sun.object) root.RemoveChild(sun.object);
    }
    if (m_FollowingPlant) root.RemoveChild(m_FollowingPlant);
    if (m_PreviewPlant) root.RemoveChild(m_PreviewPlant);
    if (m_EnergyText) root.RemoveChild(m_EnergyText);
    if (m_GameOverWord) root.RemoveChild(m_GameOverWord);
    if (m_GameOverBoard) root.RemoveChild(m_GameOverBoard);
    if (m_GameOverButton) root.RemoveChild(m_GameOverButton);
    if (m_DroppedRewardCard) root.RemoveChild(m_DroppedRewardCard);
    if (m_GetPlantBoard) root.RemoveChild(m_GetPlantBoard);
    if (m_RewardCardDisplay) root.RemoveChild(m_RewardCardDisplay);
    if (m_NextLevelButton) root.RemoveChild(m_NextLevelButton);

    m_Buttons.clear();
    m_Cards.clear();
    m_CardVisuals.clear();
    m_Zombies.clear();
    m_LawnMowers.clear();
    m_PlacedPlants.clear();
    m_BeanProjectiles.clear();
    m_SunEnergies.clear();  // Clear sun energy objects
    m_FollowingPlant = nullptr;
    m_PreviewPlant = nullptr;
    m_SelectedCard = nullptr;
    m_EnergyText = nullptr;
    m_EnergyTextPtr = nullptr;
    m_GameOverWord = nullptr;
    m_GameOverBoard = nullptr;
    m_GameOverButton = nullptr;
    m_DroppedRewardCard = nullptr;
    m_GetPlantBoard = nullptr;
    m_RewardCardDisplay = nullptr;
    m_NextLevelButton = nullptr;
    m_IntroDone = false; // ?蔭 intro ???

    // Reset energy and timers for new level
    m_EnergyCollected = 0;  // Reset collected energy
    m_ElapsedTime = 0.0f;   // Reset elapsed time
    m_WinDelayTimer = 0.0f; // Reset win delay
    m_LoseDelayTimer = 0.0f; // Reset lose delay
    m_IsGameOver = false;
    m_GameOverPanelShown = false;
    m_GameOverScaleTimer = 0.0f;
    m_GameOverShakeTimer = 0.0f;
    m_GameOverBasePosition = {0.0f, 0.0f};
    m_IsLevelClearRewardActive = false;
    m_IsRewardCardLanded = false;
    m_IsRewardPanelShown = false;
    m_RewardClaimed = false;
    m_LastZombieDeathPosition = {0.0f, 0.0f};
    m_RewardDropVelocity = {0.0f, 0.0f};
    m_RewardPlantName.clear();

    m_CurrentLevel = level;
    LoadLevel(root);
}

void LevelManager::SkipToNextLevel(Util::Renderer& root) {
    ChangeLevel(m_CurrentLevel + 1, root);
}
