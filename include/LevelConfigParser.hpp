#ifndef LEVEL_CONFIG_PARSER_HPP
#define LEVEL_CONFIG_PARSER_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "LevelConfig.hpp"

// Forward declaration for JsonValue
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

class LevelConfigParser {
public:
    static std::vector<LevelConfig> LoadFromFile(const std::string& filePath);
    static LevelConfig ParseLevel(const std::string& jsonString, int levelNumber);
    static LevelConfig ParseLevel(const JsonValue& levelJson, int levelNumber);
};

#endif
