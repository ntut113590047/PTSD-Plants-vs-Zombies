#include "LevelManager.hpp"
#include "Util/Input.hpp"

LevelManager::LevelManager(int level)
    : m_CurrentLevel(level) {}

void LevelManager::LoadLevel(Util::Renderer& root) {

    switch (m_CurrentLevel) {

        case 0: {

            m_Background = std::make_shared<Util::GameObject>(
                std::make_shared<Util::Image>(
                    RESOURCE_DIR"/Image/Background/startBackGround.png"
                ),
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

            break;
        }

        case 1: {

            m_Background = std::make_shared<Util::GameObject>(
                std::make_shared<Util::Image>(
                    RESOURCE_DIR"/Image/Background/phase1.png"
                ),
                -10
            );

            root.AddChild(m_Background);

            break;
        }
    }
}

void LevelManager::Update(Util::Renderer& root) {

    auto mousePos = Util::Input::GetCursorPosition();

    for (auto& btn : m_Buttons) {

        btn->UpdateHoverState(mousePos.x, mousePos.y);

        if (btn->IsClicked()) {
            ChangeLevel(1, root);
             break;
        }
    }

}
void LevelManager::ChangeLevel(int level, Util::Renderer& root) {

    // 移除背景
    if (m_Background) {
        root.RemoveChild(m_Background);
        m_Background = nullptr;
    }

    // 移除所有按鈕
    for (auto& btn : m_Buttons) {
        root.RemoveChild(btn);
    }
    m_Buttons.clear();

    m_CurrentLevel = level;

    LoadLevel(root);
}