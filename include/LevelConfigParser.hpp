#ifndef LEVEL_CONFIG_PARSER_HPP
#define LEVEL_CONFIG_PARSER_HPP

#include <string>
#include <vector>
#include <memory>
#include "LevelConfig.hpp"

class LevelConfigParser {
public:
    static std::vector<LevelConfig> LoadFromFile(const std::string& filePath);
    static LevelConfig ParseLevel(const std::string& jsonString, int levelNumber);
};

#endif
