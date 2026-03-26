#include "GameStateManager.hpp"

GameStateManager::GameStateManager() = default;

void GameStateManager::SetTotalWaves(int count) {
    m_TotalWaves = count;
}

void GameStateManager::AdvanceWave() {
    if (m_CurrentWave < m_TotalWaves) {
        m_CurrentWave++;
    }
}

int GameStateManager::GetCurrentWave() const {
    return m_CurrentWave;
}

int GameStateManager::GetTotalWaves() const {
    return m_TotalWaves;
}

bool GameStateManager::IsLastWave() const {
    return m_CurrentWave == m_TotalWaves;
}

void GameStateManager::RegisterZombieSpawned() {
    m_ZombiesSpawned++;
}

void GameStateManager::RegisterZombieDied() {
    m_ZombiesDead++;
}

int GameStateManager::GetZombiesSpawned() const {
    return m_ZombiesSpawned;
}

int GameStateManager::GetZombiesDead() const {
    return m_ZombiesDead;
}

int GameStateManager::GetZombiesAlive() const {
    return m_ZombiesSpawned - m_ZombiesDead;
}

void GameStateManager::RegisterZombieBreach() {
    m_ZombieBreaches++;
}

int GameStateManager::GetZombieBreaches() const {
    return m_ZombieBreaches;
}

void GameStateManager::SetWinCondition(const WinCondition& condition) {
    m_WinCondition = condition;
}

bool GameStateManager::CheckWinCondition(float elapsedTime, int energyCollected) {
    if (m_HasWon || m_HasLost) {
        return m_HasWon;
    }

    bool conditionMet = false;

    switch (m_WinCondition.type) {
        case WinConditionType::DEFEAT_ALL_ZOMBIES:
            // Beat all waves and all zombies are dead
            conditionMet = IsLastWave() && GetZombiesAlive() == 0 && m_ZombiesSpawned > 0;
            break;

        case WinConditionType::TIME_SURVIVE:
            // Survive for specified time duration
            conditionMet = elapsedTime >= m_WinCondition.value;
            break;

        case WinConditionType::NO_BREACHES:
            // No zombies should breach
            conditionMet = IsLastWave() && GetZombiesAlive() == 0 && m_ZombieBreaches == 0;
            break;

        case WinConditionType::MAX_BREACHES:
            // Allow up to N breaches
            conditionMet = IsLastWave() && GetZombiesAlive() == 0 && m_ZombieBreaches <= static_cast<int>(m_WinCondition.value);
            break;

        case WinConditionType::ENERGY_COLLECT:
            // Collect specified energy
            conditionMet = energyCollected >= static_cast<int>(m_WinCondition.value);
            break;
    }

    if (conditionMet) {
        m_HasWon = true;
    }

    return m_HasWon;
}

void GameStateManager::Reset() {
    m_CurrentWave = 0;
    m_ZombiesSpawned = 0;
    m_ZombiesDead = 0;
    m_ZombieBreaches = 0;
    m_HasWon = false;
    m_HasLost = false;
}
