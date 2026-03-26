#ifndef LEVEL_CONFIG_HPP
#define LEVEL_CONFIG_HPP

#include <string>
#include <vector>
#include "GameStateManager.hpp"

struct ZombieSpawn {
    std::string type;           // "BasicZombie", "ConeZombie", etc.
    int count = 1;              // Number of zombies to spawn
    float spawn_interval = 1.0f; // Time between spawns (seconds)
    float delay_offset = 0.0f;   // Delay before first spawn
};

struct Wave {
    std::vector<ZombieSpawn> zombies;
    float start_delay = 0.0f;    // Delay before this wave starts
};

struct LevelConfig {
    int level_number = 1;
    std::string background;
    std::vector<int> rows_unlocked;           // Which rows can have plants
    std::vector<std::string> available_plants; // Which plants can be placed
    int starting_energy = 50;
    std::vector<Wave> waves;
    GameStateManager::WinCondition win_condition;
};

#endif
