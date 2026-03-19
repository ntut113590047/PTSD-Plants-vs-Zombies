#include "LevelManager.hpp"
#include "Util/Input.hpp"

LevelManager::LevelManager(int level)
    : m_CurrentLevel(level) {}

bool LevelManager::IsGameLevel() const {
    return m_CurrentLevel != 0;
}

void LevelManager::LoadLevel(Util::Renderer& root) {

    if (m_CurrentLevel == 0) {
        m_Background = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Background/startBackGround.png"),
            -10
        );
    }
    else {
        std::string path;

        switch (m_CurrentLevel) {
            case 1: path = RESOURCE_DIR"/Image/Background/phase1.png"; break;
            case 2: path = RESOURCE_DIR"/Image/Background/phase2.png"; break;
            case 3: path = RESOURCE_DIR"/Image/Background/phase3.png"; break;
            default: path = RESOURCE_DIR"/Image/Background/phase3.png"; break;
        }

        m_Background = std::make_shared<Util::GameObject>(
            std::make_shared<Util::Image>(path),
            -10
        );

        m_Background->m_Transform.scale.x = 1.1f;
        m_Background->m_Transform.scale.y = 1.1f;

        m_Background->m_Transform.translation.x = 200.0f;

        m_IntroPhase = 0;
        m_IntroDone = false;
        m_Timer = 0;
    }

    // ===== UI =====
    if (m_CurrentLevel == 0) {
        auto startButton = std::make_shared<Button>(
            RESOURCE_DIR"/Image/Other/startButton.png",
            RESOURCE_DIR"/Image/Other/startButton_2.png"
        );

        startButton->SetZIndex(5);
        startButton->SetScale(0.7f, 0.7f);
        startButton->SetPosition(260.0f, 210.0f);

        root.AddChild(startButton);
        m_Buttons.push_back(startButton);
    }

    root.AddChild(m_Background);
}

void LevelManager::Update(Util::Renderer& root) {

    auto mousePos = Util::Input::GetCursorPosition();

    // ===== 按鈕 =====
    for (auto& btn : m_Buttons) {
        btn->UpdateHoverState(mousePos.x, mousePos.y);

        if (btn->IsClicked()) {
            ChangeLevel(1, root);
            return;
        }
    }

    // ===== 🧩 卡片槽滑入動畫 =====
    if (m_CardSlotActive && m_CardSlot) {

        float speed = 15.0f; // 👉 控制滑下速度

        if (m_CardSlot->m_Transform.translation.y > m_CardSlotTargetY) {
            m_CardSlot->m_Transform.translation.y -= speed;
        } else {
            m_CardSlot->m_Transform.translation.y = m_CardSlotTargetY;
        }
    }

    // ===== 🔤 Word動畫 =====
    if (m_IntroDone && m_WordPhase > 0) {
        m_WordTimer++;

        auto LerpScale = [](float a, float b, float t) {
            return a + (b - a) * t;
        };

        switch (m_WordPhase) {
            case 1: {
                float t = m_WordTimer / 30.0f;
                if (t > 1.0f) t = 1.0f;

                m_Word->m_Transform.scale.x = LerpScale(0.2f, 0.3f, t);
                m_Word->m_Transform.scale.y = LerpScale(0.2f, 0.3f, t);

                if (m_WordTimer > 30) {
                    root.RemoveChild(m_Word);
                    m_Word = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/word2.png"),
                        10
                    );
                    root.AddChild(m_Word);

                    m_WordPhase = 2;
                    m_WordTimer = 0;
                }
                break;
            }

            case 2: {
                float t = m_WordTimer / 30.0f;
                if (t > 1.0f) t = 1.0f;

                m_Word->m_Transform.scale.x = LerpScale(0.2f, 0.3f, t);
                m_Word->m_Transform.scale.y = LerpScale(0.2f, 0.3f, t);

                if (m_WordTimer > 30) {
                    root.RemoveChild(m_Word);
                    m_Word = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/word3.png"),
                        10
                    );

                    m_Word->m_Transform.scale.x = 0.7f;
                    m_Word->m_Transform.scale.y = 0.7f;

                    root.AddChild(m_Word);

                    m_WordPhase = 3;
                    m_WordTimer = 0;
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

    // ===== 🎬 開場動畫 =====
    if (IsGameLevel() && !m_IntroDone) {

        auto EaseInOut = [](float t) {
            return t * t * (3 - 2 * t);
        };

        switch (m_IntroPhase) {

            case 0:
                m_Timer++;
                if (m_Timer > 60) {
                    m_IntroPhase = 1;
                    m_StartX = m_Background->m_Transform.translation.x;
                    m_TargetX = -200.0f;
                    m_Timer = 0;
                }
                break;

            case 1: {
                float t = m_Timer / m_Duration;
                float eased = EaseInOut(t);

                m_Background->m_Transform.translation.x =
                    m_StartX + (m_TargetX - m_StartX) * eased;

                m_Timer++;

                if (t >= 1.0f) {
                    m_IntroPhase = 2;
                    m_Timer = 0;
                }
                break;
            }

            case 2:
                m_Timer++;
                if (m_Timer > 60) {
                    m_IntroPhase = 3;
                    m_StartX = m_Background->m_Transform.translation.x;
                    m_TargetX = 180.0f;
                    m_Timer = 0;
                }
                break;

            case 3: {
                float t = m_Timer / m_Duration;
                float eased = EaseInOut(t);

                m_Background->m_Transform.translation.x =
                    m_StartX + (m_TargetX - m_StartX) * eased;

                m_Timer++;

                if (t >= 1.0f) {

                    // ⭐ 卡片槽從畫面上方掉下來
                    m_CardSlot = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/cardplot.png"),
                        10
                    );

                    m_CardSlot->m_Transform.scale.x = 0.25f;
                    m_CardSlot->m_Transform.scale.y = 0.25f;

                    m_CardSlot->m_Transform.translation.x = -300.0f;
                    m_CardSlot->m_Transform.translation.y = 500.0f; // 👉 畫面外上方

                    root.AddChild(m_CardSlot);
                    m_CardSlotActive = true;

                    // 🔤 文字
                    m_WordPhase = 1;
                    m_WordTimer = 0;

                    m_Word = std::make_shared<Util::GameObject>(
                        std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/word1.png"),
                        10
                    );

                    root.AddChild(m_Word);

                    m_IntroDone = true;
                }
                break;
            }
        }
    }
}

void LevelManager::ChangeLevel(int level, Util::Renderer& root) {

    if (m_Background) {
        root.RemoveChild(m_Background);
        m_Background = nullptr;
    }

    if (m_CardSlot) {
        root.RemoveChild(m_CardSlot);
        m_CardSlot = nullptr;
    }

    for (auto& btn : m_Buttons) {
        root.RemoveChild(btn);
    }
    m_Buttons.clear();

    m_CurrentLevel = level;

    LoadLevel(root);
}