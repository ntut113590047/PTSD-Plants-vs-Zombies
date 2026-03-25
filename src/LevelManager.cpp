#include "LevelManager.hpp"
#include "Util/Input.hpp"
#include "Util/Text.hpp"
#include "Util/Animation.hpp"
#include "Util/Time.hpp"
#include "PlantRegistry.hpp"
#include "PeashooterPlant.hpp"
#include "SunflowerPlant.hpp"
#include "BasicZombie.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

#define NUM_CARDS 6

LevelManager::LevelManager(int level)
    : m_CurrentLevel(level) {}

bool LevelManager::IsGameLevel() const {
    return m_CurrentLevel != 0;
}

void LevelManager::LoadLevel(Util::Renderer& root) {

    // ===== 起始畫面 =====
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

        return; // 起始畫面不初始化卡片或文字動畫
    }

    // ===== 正式關卡 =====
    std::string path;
    switch (m_CurrentLevel) {
        case 1: path = RESOURCE_DIR"/Image/Background/phase1.png"; break;
        case 2: path = RESOURCE_DIR"/Image/Background/phase2.png"; break;
        case 3: path = RESOURCE_DIR"/Image/Background/phase3.png"; break;
        default: path = RESOURCE_DIR"/Image/Background/phase3.png"; break;
    }

    m_Background = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(path), -10
    );
	m_Background->m_Transform.scale = {1.1f, 1.1f};
    m_Background->m_Transform.translation = {200.0f, 0.0f};
    root.AddChild(m_Background);

    // ===== 初始化文字動畫 =====
    m_WordPhase = 1;
    m_WordTimer = 0.0f;
    m_Word = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/word1.png"),
        10
    );
    m_Word->m_Transform.scale = {0.2f, 0.2f}; // 初始縮放
    m_Word->m_Transform.translation = {0.0f, 0.0f}; // 初始位置
    root.AddChild(m_Word);

    // ===== 初始化卡片槽 =====
    m_CardSlot = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/cardplot.png"),
        10
    );
    m_CardSlot->m_Transform.scale = {0.25f, 0.25f};
    m_CardSlot->m_Transform.translation = {-300.0f, m_CardSlotTargetY};
    root.AddChild(m_CardSlot);
    m_CardSlotActive = true;

    // ===== 初始化卡片位置 =====
    m_CardPositions.clear();
    float startX = m_CardSlot->m_Transform.translation.x - 150.0f;
    float startY = m_CardSlot->m_Transform.translation.y;
    float spacing = 80.0f;
    for (size_t i = 0; i < NUM_CARDS; ++i) {
        m_CardPositions.push_back(glm::vec2(startX + i * spacing, startY));
    }

    // ===== 初始化卡片資料 =====
    auto& registry = PlantRegistry::GetInstance();
    m_LevelPlants.clear();
    switch (m_CurrentLevel) {
    case 1:
        m_LevelPlants.push_back(registry.GetPlantData("bean"));
        break;

    case 2:
        m_LevelPlants.push_back(registry.GetPlantData("bean"));
        m_LevelPlants.push_back(registry.GetPlantData("sunflower"));
        break;

    case 3:
        m_LevelPlants.push_back(registry.GetPlantData("bean"));
        m_LevelPlants.push_back(registry.GetPlantData("sunflower"));
        m_LevelPlants.push_back(registry.GetPlantData("cherry"));
        break;
    case 4:
        m_LevelPlants.push_back(registry.GetPlantData("bean"));
        m_LevelPlants.push_back(registry.GetPlantData("sunflower"));
        m_LevelPlants.push_back(registry.GetPlantData("cherry"));
        m_LevelPlants.push_back(registry.GetPlantData("nut"));
        break;

    default:
        break;
}
    // ===== 創建卡片 =====
    m_Cards.clear();
    m_CardVisuals.clear();
    for (size_t i = 0; i < NUM_CARDS; ++i) {
        if (i >= m_LevelPlants.size()) break;
        auto& data = m_LevelPlants[i];
        auto card = std::make_shared<PlantCard>(data, m_CardPositions[i].x, m_CardPositions[i].y);
        card->SetZIndex(m_CardSlot->GetZIndex() + 1);
		card->m_Transform.scale = {0.6f, 0.6f};
        root.AddChild(card);
        m_Cards.push_back(card);

        // 卡片上方兩層遮罩：能量不足微暗、冷卻較暗（由底往上）
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

    // ===== 初始化草坪網格 =====
    m_GrassGrid.assign(5, std::vector<bool>(9, false)); // 5行9列，false表示空
    m_RowAllowed.assign(5, false);
    if (m_CurrentLevel == 1) {
        m_RowAllowed[2] = true; // 中間一橫排
    } else if (m_CurrentLevel == 2) {
        m_RowAllowed[1] = true;
        m_RowAllowed[2] = true;
        m_RowAllowed[3] = true; // 中間三橫排
    } else {
        for (int i = 0; i < 5; ++i) m_RowAllowed[i] = true; // 全部行可放
    }

    // ===== 初始化每行割草機 =====
    m_LawnMowers.clear();
    {
        float leftX = -300.0f;
        float topY = 180.0f;
        float bottomY = -290.0f;
        int rows = 5;
        float cellHeight = (topY - bottomY) / (rows - 1);

        for (int r = 0; r < rows; ++r) {
            if (r >= static_cast<int>(m_RowAllowed.size()) || !m_RowAllowed[r]) {
                continue;
            }
            float rowY = topY - r * cellHeight;
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
    m_PlayerEnergy = (m_CurrentLevel == 1) ? 150 : 50; // 第一關150，其他50

    m_SunEnergies.clear();
    m_BeanProjectiles.clear();
    m_SunSpawnTimer = 0.0f;
    m_Zombies.clear();
    m_ZombieSpawnTimer = 0.0f;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // ===== 初始化能量顯示文字 =====
    m_EnergyTextPtr = std::make_shared<Util::Text>("C:/PTSD-Plants-vs-Zombies/PTSD/assets/fonts/Inter.ttf", 24, std::to_string(m_PlayerEnergy), Util::Color(0, 0, 0));
    m_EnergyText = std::make_shared<Util::GameObject>(m_EnergyTextPtr, 30);
    m_EnergyText->m_Transform.translation = {-540.0f, 285.0f}; // 左上角位置，這裡修改xy座標
    root.AddChild(m_EnergyText);

    m_IntroDone = true; // 初始化完成
}

void LevelManager::Update(Util::Renderer& root, float deltaTime) {
    // ===== 起始畫面按鈕 =====
    if (m_CurrentLevel == 0) {
        auto mousePos = Util::Input::GetCursorPosition();
        for (auto& btn : m_Buttons) {
            btn->UpdateHoverState(mousePos.x, mousePos.y);
            if (btn->IsClicked()) {
                ChangeLevel(1, root); // 點開始，進入正式關卡
                return;
            }
        }
        return; // 起始畫面只處理按鈕，不做文字或卡片動畫
    }

    // ===== 正式關卡文字動畫 =====
    if (!m_IntroDone || !IsGameLevel()) return;

    if (m_WordPhase > 0 && m_Word) {
        m_WordTimer += deltaTime; // 累計時間（秒）
        auto LerpScale = [](float a, float b, float t) { return a + (b - a) * t; };

        switch (m_WordPhase) {
            case 1: {
                float t = std::min(m_WordTimer / 0.5f, 1.0f); // 0.5秒動畫
                m_Word->m_Transform.scale = { LerpScale(0.2f,0.3f,t), LerpScale(0.2f,0.3f,t) };
                if (m_WordTimer > 0.5f) {
                    root.RemoveChild(m_Word);
                    m_Word = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/word2.png"), 10
                    );
                    m_Word->m_Transform.scale = {0.2f, 0.2f}; // 設置初始縮放
                    m_Word->m_Transform.translation = {0.0f, 0.0f}; // 設置初始位置
                    root.AddChild(m_Word);
                    m_WordPhase = 2; m_WordTimer = 0.0f;
                }
                break;
            }
            case 2: {
                float t = std::min(m_WordTimer / 0.5f, 1.0f); // 0.5秒動畫
                m_Word->m_Transform.scale = { LerpScale(0.2f,0.3f,t), LerpScale(0.2f,0.3f,t) };
                if (m_WordTimer > 0.5f) {
                    root.RemoveChild(m_Word);
                    m_Word = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/word3.png"), 10
                    );
                    m_Word->m_Transform.scale = {0.7f,0.7f};
                    m_Word->m_Transform.translation = {0.0f, 0.0f}; // 設置初始位置
                    root.AddChild(m_Word);
                    m_WordPhase = 3; m_WordTimer = 0.0f;
                }
                break;
            }
            case 3:
                if (m_WordTimer > 1.17f) { // 約1.17秒顯示
                    root.RemoveChild(m_Word);
                    m_Word = nullptr;
                    m_WordPhase = 0;
                }
                break;
        }
    }

    // ===== 遊戲開始：放置植物 =====
    if (m_WordPhase == 0) {
        // 更新能量顯示
        if (m_EnergyTextPtr) {
            m_EnergyTextPtr->SetText(std::to_string(m_PlayerEnergy));
        }

        // 能量掉落產生 (每5秒)
        m_SunSpawnTimer += deltaTime; // 累計時間（秒）
        const float spawnInterval = 5.0f; // 5秒間隔
        if (m_SunSpawnTimer >= spawnInterval) {
            m_SunSpawnTimer = 0.0f;
            float minX = -540.0f;
            float maxX = 540.0f;
            float x = minX + static_cast<float>(std::rand()) / RAND_MAX * (maxX - minX);
            float y = 330.0f; // 畫面上方

            std::vector<std::string> energyPaths;
            for (int i = 1; i <= 29; ++i) {
                energyPaths.push_back(RESOURCE_DIR"/Image/Other/energy/frame_" + std::to_string(i) + ".png");
            }

            auto energyObj = std::make_shared<Util::GameObject>(
                std::make_shared<Util::Animation>(energyPaths, true, 50, true, 0), 15
            );
            energyObj->m_Transform.translation = {x, y};
            energyObj->m_Transform.scale = {1.0f, 1.0f}; // 能量2倍大
            root.AddChild(energyObj);

            float dropSpeed = -40.0f; // 落下速度固定
            float stopY = -120.0f - static_cast<float>(std::rand()) / RAND_MAX * 80.0f; // 隨機高度 -120 ~ -200
            m_SunEnergies.push_back({energyObj, {0.0f, dropSpeed}, false, stopY});
        }

        // 更新卡片冷卻與可用狀態
        for (size_t i = 0; i < m_Cards.size(); ++i) {
            auto& card = m_Cards[i];
            card->UpdateCooldown(deltaTime); // 使用真實的 deltaTime
            bool hasEnergy = (m_PlayerEnergy >= card->GetData().cost);
            // 不可用的卡片變灰, 有可用變回正常亮度
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

        // 第一關：右側畫面外生成殭屍並往左走進場
        if (m_CurrentLevel == 1) {
            m_ZombieSpawnTimer += deltaTime;
            const float spawnInterval = 4.0f;
            if (m_ZombieSpawnTimer >= spawnInterval) {
                m_ZombieSpawnTimer = 0.0f;

                std::vector<std::string> zombiePaths;
                for (int i = 1; i <= 22; ++i) {
                    zombiePaths.push_back(RESOURCE_DIR"/Image/zombies/zombie/frame_" + std::to_string(i) + ".png");
                }

                float topY = 180.0f;
                float bottomY = -290.0f;
                int rows = 5;
                float cellHeight = (topY - bottomY) / (rows - 1);

                int spawnRow = 2;
                std::vector<int> allowedRows;
                for (int r = 0; r < rows; ++r) {
                    if (r < static_cast<int>(m_RowAllowed.size()) && m_RowAllowed[r]) {
                        allowedRows.push_back(r);
                    }
                }
                if (!allowedRows.empty()) {
                    spawnRow = allowedRows[std::rand() % allowedRows.size()];
                }

                float spawnY = topY - spawnRow * cellHeight + 20.0f;
                float spawnX = 680.0f; // 可視畫面右側外
                auto zombie = std::make_shared<BasicZombie>(zombiePaths, spawnRow, 35.0f);
                zombie->m_Transform.translation = {spawnX, spawnY};
                root.AddChild(zombie);

                m_Zombies.push_back(zombie);
            }
        }

        // 戰鬥：交由植物類別偵測與攻擊（豌豆射手會在自己的 Attack 內發射投射物）
        for (auto& plant : m_PlacedPlants) {
            auto projectile = plant->Attack(m_Zombies, deltaTime);
            if (projectile.has_value() && projectile->object) {
                root.AddChild(projectile->object);
                m_BeanProjectiles.push_back({
                    projectile->object,
                    projectile->row,
                    projectile->speed,
                    projectile->damage
                });
            }
        }

        // 更新豌豆投射物：飛行、碰撞、移除
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
                    consumed = true;
                    break;
                }
            }

            if (consumed || bean.object->m_Transform.translation.x > 780.0f) {
                root.RemoveChild(bean.object);
                m_BeanProjectiles.erase(m_BeanProjectiles.begin() + i);
            }
        }

        // 割草機：偵測殭屍接觸 → 激活並橫掃同排所有殭屍
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
        // 移除已飛出右側畫面的割草機
        for (int i = static_cast<int>(m_LawnMowers.size()) - 1; i >= 0; --i) {
            if (m_LawnMowers[i]->IsActive() &&
                m_LawnMowers[i]->m_Transform.translation.x > 800.0f) {
                root.RemoveChild(m_LawnMowers[i]);
                m_LawnMowers.erase(m_LawnMowers.begin() + i);
            }
        }

        // 戰鬥：殭屍接觸植物時停下並攻擊，否則持續前進
        for (auto& zombie : m_Zombies) {
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
                zombie->SetAttacking(true);
                if (zombie->TryAttack(deltaTime)) {
                    blockingPlant->TakeDamage(zombie->GetAttackDamage());
                }
            } else {
                zombie->SetAttacking(false);
                zombie->Update(deltaTime);
            }
        }

        // 移除死亡的殭屍或越界殭屍
        for (int i = static_cast<int>(m_Zombies.size()) - 1; i >= 0; --i) {
            auto& z = m_Zombies[i];
            if (z->IsDead() || z->m_Transform.translation.x < -760.0f) {
                root.RemoveChild(z);
                m_Zombies.erase(m_Zombies.begin() + i);
            }
        }

        // 移除死亡植物並釋放格子
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

        // 更新能量掉落、撿取
        {
            glm::vec2 targetPos = {-540.0f, 300.0f}; // 能量文字上方一點位置
            float deltaSeconds = deltaTime; // 使用真實的 deltaTime
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
                        continue;
                    }
                    glm::vec2 dir = diff / dist;
                    float collectSpeed = std::max(200.0f, std::min(1200.0f, dist * 6.0f));
                    obj->m_Transform.translation += dir * collectSpeed * deltaSeconds; // 從快到慢
                } else {
                    obj->m_Transform.translation += sun.velocity * deltaSeconds;

                    if (obj->m_Transform.translation.y < sun.stopY) {
                        obj->m_Transform.translation.y = sun.stopY;
                        sun.velocity = {0.0f, 0.0f};
                    }

                    // 如果飛出底部就刪除（保險）
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


        // 檢查卡片點擊
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB) && !m_SelectedCard) {
            for (auto& card : m_Cards) {
                if (card->IsReady() && m_PlayerEnergy >= card->GetData().cost) {
                    // 計算實際大小
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

        // 更新跟隨植物位置
        if (m_FollowingPlant) {
            m_FollowingPlant->m_Transform.translation = {mousePos.x, mousePos.y};

            // 檢查預覽位置
            int previewRow = -1, previewCol = -1;
            float leftX = -300.0f;
            float rightX = 500.0f;
            float topY = 180.0f;
            float bottomY = -290.0f;
            int cols = 9;
            int rows = 5;
            float cellWidth = (rightX - leftX) / (cols - 1);
            float cellHeight = (topY - bottomY) / (rows - 1);

            for (int r = 0; r < rows; ++r) {
                for (int c = 0; c < cols; ++c) {
                    float cellX = leftX + c * cellWidth;
                    float cellY = topY - r * cellHeight;
                    float halfSizeX = cellWidth / 2.0f;
                    float halfSizeY = cellHeight / 2.0f;
                    if (mousePos.x >= cellX - halfSizeX && mousePos.x < cellX + halfSizeX &&
                        mousePos.y <= cellY + halfSizeY && mousePos.y > cellY - halfSizeY) {
                        previewRow = r;
                        previewCol = c;
                        break;
                    }
                }
                if (previewRow != -1) break;
            }

            // 更新預覽植物
            if (previewRow != -1 && previewCol != -1 && m_RowAllowed[previewRow] && !m_GrassGrid[previewRow][previewCol]) {
                if (!m_PreviewPlant) {
                    // 創建預覽植物（半透明）
                    m_PreviewPlant = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Animation>(m_SelectedCard->GetData().plantAnimationPaths, true, 50, true, 0), 10
                    );
                    m_PreviewPlant->m_Transform.scale = {m_SelectedCard->GetData().scale, m_SelectedCard->GetData().scale};
                    // 設置半透明（如果支持的話）
                    root.AddChild(m_PreviewPlant);
                }
                float cellX = leftX + previewCol * cellWidth;
                float cellY = topY - previewRow * cellHeight;
                m_PreviewPlant->m_Transform.translation = {cellX, cellY};
            } else {
                // 移除預覽植物
                if (m_PreviewPlant) {
                    root.RemoveChild(m_PreviewPlant);
                    m_PreviewPlant = nullptr;
                }
            }
        } else {
            // 沒有跟隨植物時也要清理預覽
            if (m_PreviewPlant) {
                root.RemoveChild(m_PreviewPlant);
                m_PreviewPlant = nullptr;
            }
        }

        // 檢查放置
        if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB) && m_SelectedCard) {
            // 如果有預覽植物且位置有效，直接使用預覽位置
            if (m_PreviewPlant) {
                // 找到對應的格子坐標
                float leftX = -300.0f;
                float rightX = 500.0f;
                float topY = 180.0f;
                float bottomY = -290.0f;
                int cols = 9;
                int rows = 5;
                float cellWidth = (rightX - leftX) / (cols - 1);
                float cellHeight = (topY - bottomY) / (rows - 1);

                glm::vec2 previewPos = m_PreviewPlant->m_Transform.translation;
                int row = -1, col = -1;

                for (int r = 0; r < rows; ++r) {
                    for (int c = 0; c < cols; ++c) {
                        float cellX = leftX + c * cellWidth;
                        float cellY = topY - r * cellHeight;
                        if (std::abs(previewPos.x - cellX) < 1.0f && std::abs(previewPos.y - cellY) < 1.0f) {
                            row = r;
                            col = c;
                            break;
                        }
                    }
                    if (row != -1) break;
                }

                if (row != -1 && col != -1 && m_RowAllowed[row] && !m_GrassGrid[row][col]) {
                    // 依卡片建立對應植物類別
                    std::shared_ptr<Plant> placedPlant;
                    const auto& data = m_SelectedCard->GetData();
                    if (data.name == "bean" || data.name == "peashooter") {
                        placedPlant = std::make_shared<PeashooterPlant>(data);
                    } else if (data.name == "sunflower") {
                        placedPlant = std::make_shared<SunflowerPlant>(data);
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

                    // 清理
                    m_PreviewPlant = nullptr;
                    root.RemoveChild(m_FollowingPlant);
                    m_FollowingPlant = nullptr;
                    m_SelectedCard = nullptr;
                }
            } else {
                // 沒有預覽植物，取消選擇
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
    if (m_FollowingPlant) root.RemoveChild(m_FollowingPlant);
    if (m_PreviewPlant) root.RemoveChild(m_PreviewPlant);
    if (m_EnergyText) root.RemoveChild(m_EnergyText);

    m_Buttons.clear();
    m_Cards.clear();
    m_CardVisuals.clear();
    m_Zombies.clear();
    m_LawnMowers.clear();
    m_PlacedPlants.clear();
    m_BeanProjectiles.clear();
    m_FollowingPlant = nullptr;
    m_PreviewPlant = nullptr;
    m_SelectedCard = nullptr;
    m_EnergyText = nullptr;
    m_EnergyTextPtr = nullptr;
    m_IntroDone = false; // 重置 intro 狀態

    m_CurrentLevel = level;
    LoadLevel(root);
}