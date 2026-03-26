#include "LevelConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

// Simple trim function
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

// Simple JSON parser - handles basic structure needed for level config
static JsonValue parseJsonValue(const std::string& json, size_t& pos) {
    JsonValue val;

    // Skip whitespace
    while (pos < json.size() && std::isspace(json[pos])) pos++;

    if (pos >= json.size()) return val;

    if (json[pos] == '{') {
        // Parse object
        val.type = JsonValue::Object;
        pos++; // skip {

        while (pos < json.size() && json[pos] != '}') {
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            if (json[pos] == '}') break;

            // Parse key
            if (json[pos] == '"') {
                pos++;
                std::string key;
                while (pos < json.size() && json[pos] != '"') {
                    key += json[pos++];
                }
                pos++; // skip closing "

                while (pos < json.size() && (std::isspace(json[pos]) || json[pos] == ':')) pos++;

                // Parse value
                JsonValue value = parseJsonValue(json, pos);
                val.object_value[key] = value;
            }

            while (pos < json.size() && (std::isspace(json[pos]) || json[pos] == ',')) pos++;
        }
        if (pos < json.size() && json[pos] == '}') pos++;
    }
    else if (json[pos] == '[') {
        // Parse array
        val.type = JsonValue::Array;
        pos++; // skip [

        while (pos < json.size() && json[pos] != ']') {
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            if (json[pos] == ']') break;

            JsonValue elem = parseJsonValue(json, pos);
            val.array_value.push_back(elem);

            while (pos < json.size() && (std::isspace(json[pos]) || json[pos] == ',')) pos++;
        }
        if (pos < json.size() && json[pos] == ']') pos++;
    }
    else if (json[pos] == '"') {
        // Parse string
        val.type = JsonValue::String;
        pos++;
        while (pos < json.size() && json[pos] != '"') {
            val.str_value += json[pos++];
        }
        if (pos < json.size()) pos++; // skip closing "
    }
    else if (json[pos] == 't' || json[pos] == 'f') {
        // Parse boolean
        val.type = JsonValue::Boolean;
        if (json.substr(pos, 4) == "true") {
            val.bool_value = true;
            pos += 4;
        } else if (json.substr(pos, 5) == "false") {
            val.bool_value = false;
            pos += 5;
        }
    }
    else {
        // Parse number
        val.type = JsonValue::Number;
        std::string numStr;
        while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '.' || json[pos] == '-')) {
            numStr += json[pos++];
        }
        val.num_value = std::stof(numStr);
    }

    while (pos < json.size() && std::isspace(json[pos])) pos++;
    return val;
}

std::vector<LevelConfig> LevelConfigParser::LoadFromFile(const std::string& filePath) {
    std::vector<LevelConfig> levels;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open level config file: " << filePath << std::endl;
        return levels;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    file.close();

    size_t pos = 0;
    JsonValue rootJson = parseJsonValue(jsonContent, pos);

    // Iterate through each level in the JSON
    for (int i = 1; i <= 10; i++) {
        std::string levelKey = "level_" + std::to_string(i);

        if (rootJson.object_value.find(levelKey) != rootJson.object_value.end()) {
            JsonValue levelJson = rootJson.object_value[levelKey];
            LevelConfig config = ParseLevel(levelJson, i);
            levels.push_back(config);
        }
    }

    return levels;
}

LevelConfig LevelConfigParser::ParseLevel(const std::string& jsonString, int levelNumber) {
    LevelConfig config;
    config.level_number = levelNumber;

    // Default values
    config.background = "phase1.png";
    config.starting_energy = 50;
    config.rows_unlocked = {0, 1, 2, 3, 4};
    config.available_plants = {"bean", "sunflower"};

    // Win condition defaults
    config.win_condition.type = GameStateManager::WinConditionType::DEFEAT_ALL_ZOMBIES;
    config.win_condition.value = 0.0f;

    // Parse based on level number (simple hardcoded setup for now)
    // This will be populated from JSON later

    return config;
}

LevelConfig LevelConfigParser::ParseLevel(const JsonValue& levelJson, int levelNumber) {
    LevelConfig config;
    config.level_number = levelNumber;

    // Default values
    config.background = "phase1.png";
    config.starting_energy = 50;
    config.rows_unlocked = {0, 1, 2, 3, 4};
    config.available_plants = {"bean", "sunflower"};
    config.win_condition.type = GameStateManager::WinConditionType::DEFEAT_ALL_ZOMBIES;
    config.win_condition.value = 0.0f;

    if (levelJson.type != JsonValue::Object) {
        return config;
    }

    // Parse background
    if (levelJson.object_value.find("background") != levelJson.object_value.end()) {
        const auto& bg = levelJson.object_value.at("background");
        if (bg.type == JsonValue::String) {
            config.background = bg.str_value;
        }
    }

    // Parse starting energy
    if (levelJson.object_value.find("starting_energy") != levelJson.object_value.end()) {
        const auto& energy = levelJson.object_value.at("starting_energy");
        if (energy.type == JsonValue::Number) {
            config.starting_energy = static_cast<int>(energy.num_value);
        }
    }

    // Parse rows_unlocked
    if (levelJson.object_value.find("rows_unlocked") != levelJson.object_value.end()) {
        const auto& rows = levelJson.object_value.at("rows_unlocked");
        if (rows.type == JsonValue::Array) {
            config.rows_unlocked.clear();
            for (const auto& row : rows.array_value) {
                if (row.type == JsonValue::Number) {
                    config.rows_unlocked.push_back(static_cast<int>(row.num_value));
                }
            }
        }
    }

    // Parse available_plants
    if (levelJson.object_value.find("available_plants") != levelJson.object_value.end()) {
        const auto& plants = levelJson.object_value.at("available_plants");
        if (plants.type == JsonValue::Array) {
            config.available_plants.clear();
            for (const auto& plant : plants.array_value) {
                if (plant.type == JsonValue::String) {
                    config.available_plants.push_back(plant.str_value);
                }
            }
        }
    }

    // Parse waves
    if (levelJson.object_value.find("waves") != levelJson.object_value.end()) {
        const auto& wavesArray = levelJson.object_value.at("waves");
        if (wavesArray.type == JsonValue::Array) {
            config.waves.clear();
            for (const auto& waveJson : wavesArray.array_value) {
                if (waveJson.type == JsonValue::Object) {
                    Wave wave;

                    // Parse start_delay
                    if (waveJson.object_value.find("start_delay") != waveJson.object_value.end()) {
                        const auto& delay = waveJson.object_value.at("start_delay");
                        if (delay.type == JsonValue::Number) {
                            wave.start_delay = delay.num_value;
                        }
                    }

                    // Parse zombies in this wave
                    if (waveJson.object_value.find("zombies") != waveJson.object_value.end()) {
                        const auto& zombiesArray = waveJson.object_value.at("zombies");
                        if (zombiesArray.type == JsonValue::Array) {
                            for (const auto& zombieJson : zombiesArray.array_value) {
                                if (zombieJson.type == JsonValue::Object) {
                                    ZombieSpawn spawn;

                                    // Parse type
                                    if (zombieJson.object_value.find("type") != zombieJson.object_value.end()) {
                                        const auto& type = zombieJson.object_value.at("type");
                                        if (type.type == JsonValue::String) {
                                            spawn.type = type.str_value;
                                        }
                                    }

                                    // Parse count
                                    if (zombieJson.object_value.find("count") != zombieJson.object_value.end()) {
                                        const auto& count = zombieJson.object_value.at("count");
                                        if (count.type == JsonValue::Number) {
                                            spawn.count = static_cast<int>(count.num_value);
                                        }
                                    }

                                    // Parse spawn_interval
                                    if (zombieJson.object_value.find("spawn_interval") != zombieJson.object_value.end()) {
                                        const auto& interval = zombieJson.object_value.at("spawn_interval");
                                        if (interval.type == JsonValue::Number) {
                                            spawn.spawn_interval = interval.num_value;
                                        }
                                    }

                                    // Parse delay_offset
                                    if (zombieJson.object_value.find("delay_offset") != zombieJson.object_value.end()) {
                                        const auto& offset = zombieJson.object_value.at("delay_offset");
                                        if (offset.type == JsonValue::Number) {
                                            spawn.delay_offset = offset.num_value;
                                        }
                                    }

                                    wave.zombies.push_back(spawn);
                                }
                            }
                        }
                    }

                    config.waves.push_back(wave);
                }
            }
        }
    }

    // Parse win_condition
    if (levelJson.object_value.find("win_condition") != levelJson.object_value.end()) {
        const auto& winCondJson = levelJson.object_value.at("win_condition");
        if (winCondJson.type == JsonValue::Object) {
            if (winCondJson.object_value.find("type") != winCondJson.object_value.end()) {
                const auto& typeVal = winCondJson.object_value.at("type");
                if (typeVal.type == JsonValue::String) {
                    if (typeVal.str_value == "DEFEAT_ALL_ZOMBIES") {
                        config.win_condition.type = GameStateManager::WinConditionType::DEFEAT_ALL_ZOMBIES;
                    } else if (typeVal.str_value == "TIME_SURVIVE") {
                        config.win_condition.type = GameStateManager::WinConditionType::TIME_SURVIVE;
                    } else if (typeVal.str_value == "NO_BREACHES") {
                        config.win_condition.type = GameStateManager::WinConditionType::NO_BREACHES;
                    } else if (typeVal.str_value == "MAX_BREACHES") {
                        config.win_condition.type = GameStateManager::WinConditionType::MAX_BREACHES;
                    } else if (typeVal.str_value == "ENERGY_COLLECT") {
                        config.win_condition.type = GameStateManager::WinConditionType::ENERGY_COLLECT;
                    }
                }
            }

            if (winCondJson.object_value.find("value") != winCondJson.object_value.end()) {
                const auto& valVal = winCondJson.object_value.at("value");
                if (valVal.type == JsonValue::Number) {
                    config.win_condition.value = valVal.num_value;
                }
            }
        }
    }

    return config;
}
