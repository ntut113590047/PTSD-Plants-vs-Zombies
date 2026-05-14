#include "PlantRegistry.hpp"
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

// Helper function to auto-detect frame count in a directory
static int GetFrameCountForPath(const std::string& basePath) {
    int maxFrame = 0;
    try {
        for (int i = 1; i <= 999; ++i) {
            std::string framePath = basePath + "/frame_" + std::to_string(i) + ".png";
            if (fs::exists(framePath)) {
                maxFrame = i;
            } else if (i > maxFrame + 10) {
                // Stop searching after 10 consecutive misses
                break;
            }
        }
    } catch (...) {
        // If filesystem check fails, return 0
    }
    return (maxFrame > 0) ? maxFrame : 1; // Default to 1 if nothing found
}

// Helper function to generate frame paths for any plant
static std::vector<std::string> GetFramePaths(const std::string& resourcePath, int frameCount) {
    std::vector<std::string> paths;
    for (int i = 1; i <= frameCount; ++i) {
        paths.push_back(resourcePath + "/frame_" + std::to_string(i) + ".png");
    }
    return paths;
}

PlantRegistry& PlantRegistry::GetInstance() {
    static PlantRegistry instance;
    return instance;
}

PlantRegistry::PlantRegistry() {
    // Bean 動畫幀 - auto-detect frame count
    {
        std::string basePath = RESOURCE_DIR"/Image/Plants/bean";
        int frameCount = GetFrameCountForPath(basePath);
        auto paths = GetFramePaths(basePath, frameCount);
        m_PlantDataMap["bean"] = {
            "bean", 100, 7.5f,
            RESOURCE_DIR"/Image/Plants/beanCard.png",
            paths,
            1.0f
        };
    }

    // Sunflower - energy plant (with animation frames)
    {
        std::string basePath = RESOURCE_DIR"/Image/Plants/sunflower";
        int frameCount = GetFrameCountForPath(basePath);
        auto paths = GetFramePaths(basePath, frameCount);
        // If no frames found, use card image as fallback
        if (paths.empty()) {
            paths.push_back(RESOURCE_DIR"/Image/Plants/sunflowerCard.png");
        }
        m_PlantDataMap["sunflower"] = {
            "sunflower", 50, 7.5f,
            RESOURCE_DIR"/Image/Plants/sunflowerCard.png",
            paths,
            1.0f
        };
    }

    // Walnut - defensive plant
    {
        std::string basePath = RESOURCE_DIR"/Image/Plants/wallnut";
        int frameCount = GetFrameCountForPath(basePath);
        auto paths = GetFramePaths(basePath, frameCount);
        m_PlantDataMap["walnut"] = {
            "walnut", 50, 30.0f,
            RESOURCE_DIR"/Image/Plants/nutCard.png",
            paths,
            1.0f
        };
    }

    // Potato Mine - explosive plant
    {
        std::string basePath = RESOURCE_DIR"/Image/Plants/potato";
        int frameCount = GetFrameCountForPath(basePath);
        auto paths = GetFramePaths(basePath, frameCount);
        m_PlantDataMap["potato"] = {
            "potato", 25, 30.0f,
            RESOURCE_DIR"/Image/Plants/potatoCard.png",
            paths,
            1.0f
        };
    }

    // Repeater - double shooter
    {
        std::string basePath = RESOURCE_DIR"/Image/Plants/repeater";
        int frameCount = GetFrameCountForPath(basePath);
        auto paths = GetFramePaths(basePath, frameCount);
        m_PlantDataMap["repeater"] = {
            "repeater", 200, 1.0f,
            RESOURCE_DIR"/Image/Plants/doubleBeanCard.png",
            paths,
            1.0f
        };
    }

    // Cherry Bomb - explosive plant
    {
        std::string basePath = RESOURCE_DIR"/Image/Plants/cherryBomb";
        int frameCount = GetFrameCountForPath(basePath);
        auto paths = GetFramePaths(basePath, frameCount);
        m_PlantDataMap["cherry"] = {
            "cherry", 150, 50.0f,
            RESOURCE_DIR"/Image/Plants/cherryCard.png",
            paths,
            0.7f
        };
    }

    // Snow Pea - ice plant
    {
        std::string basePath = RESOURCE_DIR"/Image/Plants/snowPea";
        int frameCount = GetFrameCountForPath(basePath);
        auto paths = GetFramePaths(basePath, frameCount);
        m_PlantDataMap["snowpea"] = {
            "snowpea", 175, 7.5f,
            RESOURCE_DIR"/Image/Plants/coldBeanCard.png",
            paths,
            1.0f
        };
    }
    // Chomper - eating plant
    {
        std::string basePath = RESOURCE_DIR"/Image/Plants/bigMouse";
        int frameCount = GetFrameCountForPath(basePath);
        auto paths = GetFramePaths(basePath, frameCount);
        m_PlantDataMap["chomper"] = {
            "chomper", 150, 15.0f,
            RESOURCE_DIR"/Image/Plants/bigMouseCard.png",
            paths,
            1.0f
        };
    }

    m_PlantDataMap["nut"] = {
        "nut", 50, 10.0f,
        RESOURCE_DIR"/Image/Plants/nutCard.png",
        {RESOURCE_DIR"/Image/Plants/nutCard.png"},
        0.5f
    };

    // Rolling Walnut (Level 5 conveyor mode)
    {
        std::string basePath = RESOURCE_DIR"/Image/Plants/nutWall_Roll";
        int frameCount = GetFrameCountForPath(basePath);
        auto paths = GetFramePaths(basePath, frameCount);
        m_PlantDataMap["nutWall"] = {
            "nutWall", 0, 0.0f,
            RESOURCE_DIR"/Image/Plants/nutCard.png",
            paths,
            0.5f
        };
    }

    // Red Rolling Walnut (Level 5 conveyor mode)
    {
        std::string basePath = RESOURCE_DIR"/Image/Plants/redNutWall_Roll";
        int frameCount = GetFrameCountForPath(basePath);
        auto paths = GetFramePaths(basePath, frameCount);
        m_PlantDataMap["redNutWall"] = {
            "redNutWall", 0, 0.0f,
            RESOURCE_DIR"/Image/Plants/redNutCard.png",
            paths,
            1.0f
        };
    }
}

PlantData PlantRegistry::GetPlantData(const std::string& plantName) const {
    auto it = m_PlantDataMap.find(plantName);
    if (it != m_PlantDataMap.end()) {
        return it->second;
    }
    // 返回默認或拋異常
    return {"unknown", 0, 0.0f, "", {}, 0.5f};
}