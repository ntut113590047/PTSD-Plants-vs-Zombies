#ifndef GAME_STATE_MANAGER_HPP
#define GAME_STATE_MANAGER_HPP

#include <memory>
#include <vector>

class GameStateManager {
public:
    enum class WinConditionType {
        DEFEAT_ALL_ZOMBIES,    // Defeat all zombies in all waves
        TIME_SURVIVE,          // Survive for specified duration
        NO_BREACHES,           // Don't let any zombies pass
        MAX_BREACHES,          // Don't let more than N zombies pass
        ENERGY_COLLECT,        // Collect specified energy amount
    };

    struct WinCondition {
        WinConditionType type;
        float value = 0.0f;    // Used for time, max breaches, energy, etc.
    };

    GameStateManager();

    // Wave management
    void SetTotalWaves(int count);
    void AdvanceWave();
    int GetCurrentWave() const;
    int GetTotalWaves() const;
    bool IsLastWave() const;

    // Zombie tracking
    void RegisterZombieSpawned();
    void RegisterZombieDied();
    int GetZombiesSpawned() const;
    int GetZombiesDead() const;
    int GetZombiesAlive() const;

    // Breach/loss tracking
    void RegisterZombieBreach();
    int GetZombieBreaches() const;
    bool HasLost() const { return m_HasLost; }

    // Win condition
    void SetWinCondition(const WinCondition& condition);
    bool CheckWinCondition(float elapsedTime, int energyCollected);
    bool HasWon() const { return m_HasWon; }

    // Game state
    bool IsGameOver() const { return m_HasWon || m_HasLost; }
    void Reset();

private:
    int m_CurrentWave = 0;
    int m_TotalWaves = 1;

    int m_ZombiesSpawned = 0;
    int m_ZombiesDead = 0;
    int m_ZombieBreaches = 0;

    WinCondition m_WinCondition;
    bool m_HasWon = false;
    bool m_HasLost = false;
};

#endif
