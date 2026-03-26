#include "PlantRegistry.hpp"
#include <string>

PlantRegistry& PlantRegistry::GetInstance() {
    static PlantRegistry instance;
    return instance;
}

PlantRegistry::PlantRegistry() {
    // 預定義植物資料
    // Bean 動畫幀
    std::vector<std::string> beanAnimationPaths;
    for (int i = 1; i <= 25; ++i) {
        beanAnimationPaths.push_back(RESOURCE_DIR"/Image/Plants/bean/frame_" + std::to_string(i) + ".png");
    }

    m_PlantDataMap["bean"] = {
        "bean", 100, 5.0f,
        RESOURCE_DIR"/Image/Plants/beanCard.png",
        beanAnimationPaths,
        1.0f
    };

    m_PlantDataMap["sunflower"] = {
        "sunflower", 50, 7.0f,
        RESOURCE_DIR"/Image/Plants/sunflowerCard.png",
        {RESOURCE_DIR"/Image/Plants/sunflowerCard.png"},
        0.5f
    };

    // Walnut - defensive plant
    std::vector<std::string> walnutFrames;
    for (int i = 1; i <= 8; ++i) {
        walnutFrames.push_back(RESOURCE_DIR"/Image/Plants/walnut/frame_" + std::to_string(i) + ".png");
    }
    m_PlantDataMap["walnut"] = {
        "walnut", 50, 8.0f,
        RESOURCE_DIR"/Image/Plants/walnut/frame_1.png",
        walnutFrames,
        1.0f
    };

    // Potato Mine - explosive plant
    std::vector<std::string> potatoFrames;
    for (int i = 1; i <= 8; ++i) {
        potatoFrames.push_back(RESOURCE_DIR"/Image/Plants/potato/frame_" + std::to_string(i) + ".png");
    }
    m_PlantDataMap["potato"] = {
        "potato", 25, 6.0f,
        RESOURCE_DIR"/Image/Plants/potato/frame_1.png",
        potatoFrames,
        0.8f
    };

    // Repeater - double shooter
    std::vector<std::string> repeaterFrames;
    for (int i = 1; i <= 8; ++i) {
        repeaterFrames.push_back(RESOURCE_DIR"/Image/Plants/repeater/frame_" + std::to_string(i) + ".png");
    }
    m_PlantDataMap["repeater"] = {
        "repeater", 200, 1.0f,
        RESOURCE_DIR"/Image/Plants/repeater/frame_1.png",
        repeaterFrames,
        1.0f
    };

    // Squash - jumping plant
    std::vector<std::string> squashFrames;
    for (int i = 1; i <= 8; ++i) {
        squashFrames.push_back(RESOURCE_DIR"/Image/Plants/squash/frame_" + std::to_string(i) + ".png");
    }
    m_PlantDataMap["squash"] = {
        "squash", 150, 4.0f,
        RESOURCE_DIR"/Image/Plants/squash/frame_1.png",
        squashFrames,
        0.9f
    };

    // Cherry Bomb - explosive plant
    std::vector<std::string> cherryFrames;
    for (int i = 1; i <= 8; ++i) {
        cherryFrames.push_back(RESOURCE_DIR"/Image/Plants/cherry/frame_" + std::to_string(i) + ".png");
    }
    m_PlantDataMap["cherry"] = {
        "cherry", 50, 6.0f,
        RESOURCE_DIR"/Image/Plants/cherry/frame_1.png",
        cherryFrames,
        0.9f
    };

    // Snow Pea - ice plant
    std::vector<std::string> snowpeaFrames;
    for (int i = 1; i <= 8; ++i) {
        snowpeaFrames.push_back(RESOURCE_DIR"/Image/Plants/snowpea/frame_" + std::to_string(i) + ".png");
    }
    m_PlantDataMap["snowpea"] = {
        "snowpea", 175, 1.5f,
        RESOURCE_DIR"/Image/Plants/snowpea/frame_1.png",
        snowpeaFrames,
        0.8f
    };

    // Chomper - eating plant
    std::vector<std::string> chomperFrames;
    for (int i = 1; i <= 8; ++i) {
        chomperFrames.push_back(RESOURCE_DIR"/Image/Plants/chomper/frame_" + std::to_string(i) + ".png");
    }
    m_PlantDataMap["chomper"] = {
        "chomper", 150, 15.0f,
        RESOURCE_DIR"/Image/Plants/chomper/frame_1.png",
        chomperFrames,
        1.0f
    };

    m_PlantDataMap["nut"] = {
        "nut", 50, 10.0f,
        RESOURCE_DIR"/Image/Plants/nutCard.png",
        {RESOURCE_DIR"/Image/Plants/nutCard.png"},
        0.5f
    };
}

PlantData PlantRegistry::GetPlantData(const std::string& plantName) const {
    auto it = m_PlantDataMap.find(plantName);
    if (it != m_PlantDataMap.end()) {
        return it->second;
    }
    // 返回默認或拋異常
    return {"unknown", 0, 0.0f, "", {}, 0.5f};
}