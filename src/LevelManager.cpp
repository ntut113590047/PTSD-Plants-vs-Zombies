#include "LevelManager.hpp"
#include "Util/Input.hpp"
#include <iostream>

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
    root.AddChild(m_Background);

    // ===== 初始化文字動畫 =====
    m_WordPhase = 1;
    m_WordTimer = 0;
    m_Word = std::make_shared<Util::GameObject>(
        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/word1.png"),
        10
    );
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
    m_LevelPlants.clear();
    switch (m_CurrentLevel) {
    case 1:
        m_LevelPlants.push_back({
            "beanCard", 100, 5.0f, RESOURCE_DIR"/Image/Plants/beanCard.png"
        });
        break;

    case 2:
        m_LevelPlants.push_back({
            "beanCard", 100, 5.0f, RESOURCE_DIR"/Image/Plants/beanCard.png"
        });
        m_LevelPlants.push_back({
            "sunflowerCard", 50, 7.0f, RESOURCE_DIR"/Image/Plants/sunflowerCard.png"
        });
        break;

    case 3:
        m_LevelPlants.push_back({
            "beanCard", 100, 5.0f, RESOURCE_DIR"/Image/Plants/beanCard.png"
        });
        m_LevelPlants.push_back({
            "sunflowerCard", 50, 7.0f, RESOURCE_DIR"/Image/Plants/sunflowerCard.png"
        });
        m_LevelPlants.push_back({
            "cherryCard", 50, 10.0f, RESOURCE_DIR"/Image/Plants/cherryCard.png"
        });
        break;
	case 4:
        m_LevelPlants.push_back({
            "beanCard", 100, 5.0f, RESOURCE_DIR"/Image/Plants/beanCard.png"
        });
        m_LevelPlants.push_back({
            "sunflowerCard", 50, 7.0f, RESOURCE_DIR"/Image/Plants/sunflowerCard.png"
        });
        m_LevelPlants.push_back({
            "cherryCard", 50, 10.0f, RESOURCE_DIR"/Image/Plants/cherryCard.png"
        });
		m_LevelPlants.push_back({
            "nutCard", 50, 10.0f, RESOURCE_DIR"/Image/Plants/nutCard.png"
        });
        break;

    default:
        break;
}
    // ===== 創建卡片 =====
    m_Cards.clear();
    for (size_t i = 0; i < NUM_CARDS; ++i) {
        if (i >= m_LevelPlants.size()) break;
        auto& data = m_LevelPlants[i];
        auto card = std::make_shared<PlantCard>(data, m_CardPositions[i].x, m_CardPositions[i].y);
        card->SetZIndex(m_CardSlot->GetZIndex() + 1);
		card->m_Transform.scale = {0.6f, 0.6f};
        root.AddChild(card);
        m_Cards.push_back(card);
    }

    m_IntroDone = true; // 初始化完成
}

void LevelManager::Update(Util::Renderer& root) {
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
        m_WordTimer++;
        auto LerpScale = [](float a, float b, float t) { return a + (b - a) * t; };

        switch (m_WordPhase) {
            case 1: {
                float t = m_WordTimer / 30.0f;
                if (t > 1.0f) t = 1.0f;
                m_Word->m_Transform.scale = { LerpScale(0.2f,0.3f,t), LerpScale(0.2f,0.3f,t) };
                if (m_WordTimer > 30) {
                    root.RemoveChild(m_Word);
                    m_Word = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/word2.png"), 10
                    );
                    root.AddChild(m_Word);
                    m_WordPhase = 2; m_WordTimer = 0;
                }
                break;
            }
            case 2: {
                float t = m_WordTimer / 30.0f;
                if (t > 1.0f) t = 1.0f;
                m_Word->m_Transform.scale = { LerpScale(0.2f,0.3f,t), LerpScale(0.2f,0.3f,t) };
                if (m_WordTimer > 30) {
                    root.RemoveChild(m_Word);
                    m_Word = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/word3.png"), 10
                    );
                    m_Word->m_Transform.scale = {0.7f,0.7f};
                    root.AddChild(m_Word);
                    m_WordPhase = 3; m_WordTimer = 0;
                }
                break;
            }
            case 3:
                if (m_WordTimer > 70) {
                    root.RemoveChild(m_Word);
                    m_Word = nullptr;
                    m_WordPhase = 0;
                }
                break;
        }
    }
}

void LevelManager::ChangeLevel(int level, Util::Renderer& root) {
    if (m_Background) { root.RemoveChild(m_Background); m_Background = nullptr; }
    if (m_CardSlot) { root.RemoveChild(m_CardSlot); m_CardSlot = nullptr; }
    for (auto& btn : m_Buttons) root.RemoveChild(btn);
    for (auto& card : m_Cards) root.RemoveChild(card);

    m_Buttons.clear();
    m_Cards.clear();

    m_CurrentLevel = level;
    LoadLevel(root);
}