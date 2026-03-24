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
        "bean", 100, 0.0f, 
        RESOURCE_DIR"/Image/Plants/beanCard.png",
        beanAnimationPaths,
        1.0f // bean 顯示大小
    };

    m_PlantDataMap["sunflower"] = {
        "sunflower", 50, 7.0f, 
        RESOURCE_DIR"/Image/Plants/sunflowerCard.png",
        {RESOURCE_DIR"/Image/Plants/sunflowerCard.png"}, // 暫用單幀
        0.5f // sunflower 顯示大小
    };

    m_PlantDataMap["cherry"] = {
        "cherry", 50, 10.0f, 
        RESOURCE_DIR"/Image/Plants/cherryCard.png",
        {RESOURCE_DIR"/Image/Plants/cherryCard.png"},
        0.5f // cherry 顯示大小
    };

    m_PlantDataMap["nut"] = {
        "nut", 50, 10.0f, 
        RESOURCE_DIR"/Image/Plants/nutCard.png",
        {RESOURCE_DIR"/Image/Plants/nutCard.png"},
        0.5f // nut 顯示大小
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