#include "LevelConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

// Simple JSON value class for basic parsing
class JsonValue {
public:
    enum Type { Null, String, Number, Array, Object, Boolean };

    Type type = Null;
    std::string str_value;
    float num_value = 0.0f;
    bool bool_value = false;
    std::vector<JsonValue> array_value;
    std::map<std::string, JsonValue> object_value;
};

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
            LevelConfig config = ParseLevel(levelJson.str_value, i);
            config.level_number = i;
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
