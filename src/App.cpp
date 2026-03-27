#include "App.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void App::Start() {

    m_LevelManager = std::make_shared<LevelManager>(0);

    m_LevelManager->LoadLevel(m_Root);

    m_CurrentState = State::UPDATE;
}

void App::Update(float deltaTime) {
    m_LevelManager->Update(m_Root, deltaTime);

    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }

    // Debug: Press T to skip to next level
    if (Util::Input::IsKeyUp(Util::Keycode::T)) {
        m_LevelManager->SkipToNextLevel(m_Root);
    }

    m_Root.Update();
}

void App::End() {
    LOG_TRACE("End");
}