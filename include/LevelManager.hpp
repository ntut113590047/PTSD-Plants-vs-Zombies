#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include <memory>
#include <vector>
#include <string>
#include "Util/GameObject.hpp"
#include "Util/Renderer.hpp"
#include "Button.hpp"

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

    std::shared_ptr<Util::GameObject> m_Background;
    std::vector<std::shared_ptr<Button>> m_Buttons;

    // 🎬 開場動畫
    int m_IntroPhase = 0;
    bool m_IntroDone = false;

    float m_Timer = 0.0f;
    float m_Duration = 120.0f;

    float m_StartX = 0.0f;
    float m_TargetX = 0.0f;

    // 🔤 Word動畫
    int m_WordPhase = 0;
    int m_WordTimer = 0;
    std::shared_ptr<Util::GameObject> m_Word;

    // 🧩 卡片槽
    std::shared_ptr<Util::GameObject> m_CardSlot;
    bool m_CardSlotActive = false;
    float m_CardSlotTargetY = 315.0f; // 最終位置

};

#endif