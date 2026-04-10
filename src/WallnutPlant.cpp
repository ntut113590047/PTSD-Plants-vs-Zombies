#include "WallnutPlant.hpp"

#include <filesystem>
#include <string>
#include <vector>

#include "Util/Animation.hpp"

namespace {
std::vector<std::string> BuildFramePathsFromFolder(const std::string& folderPath) {
    std::vector<std::string> paths;
    for (int i = 1; i <= 999; ++i) {
        std::string framePath = folderPath + "/frame_" + std::to_string(i) + ".png";
        if (!std::filesystem::exists(framePath)) {
            break;
        }
        paths.push_back(framePath);
    }
    return paths;
}

std::vector<std::string> BuildWallnutVariantPaths(const std::vector<std::string>& fallbackPaths,
                                                  const std::string& variantFolder) {
    const std::string variantFolderPath = std::string(RESOURCE_DIR) + "/Image/Plants/" + variantFolder;
    auto variantPaths = BuildFramePathsFromFolder(variantFolderPath);
    return variantPaths.empty() ? fallbackPaths : variantPaths;
}
}

WallnutPlant::WallnutPlant(const PlantData& data,
                           const std::shared_ptr<Core::Drawable>& drawable,
                           float zIndex,
                           float health)
    : Plant(data,
            drawable != nullptr ? drawable : std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, true, 0),
            zIndex,
            health,
            0.0f,
            0.0f) {
    m_WallnutStage1Animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
    m_WallnutStage2Animation = std::make_shared<Util::Animation>(
        BuildWallnutVariantPaths(data.plantAnimationPaths, "wallnut_2"),
        true,
        50,
        true,
        0);
    m_WallnutStage3Animation = std::make_shared<Util::Animation>(
        BuildWallnutVariantPaths(data.plantAnimationPaths, "wallnut_3"),
        true,
        50,
        true,
        0);

    UpdateDamageVisual();
}

void WallnutPlant::Update(float deltaTime) {
    Plant::Update(deltaTime);
    UpdateDamageVisual();
}

void WallnutPlant::TakeDamage(float damage) {
    Plant::TakeDamage(damage);
    UpdateDamageVisual();
}

bool WallnutPlant::DetectZombieInFront(const std::vector<std::shared_ptr<Zombie>>& zombies) const {
    // Walnut doesn't attack
    return false;
}

std::optional<PlantProjectile> WallnutPlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    // Walnut doesn't attack, only provides defense
    return std::nullopt;
}

void WallnutPlant::UpdateDamageVisual() {
    int targetStage = 1;
    if (GetHealth() <= 1333.0f) {
        targetStage = 3;
    } else if (GetHealth() <= 2666.0f) {
        targetStage = 2;
    }

    if (targetStage == m_CurrentVisualStage) {
        return;
    }

    std::shared_ptr<Util::Animation> sourceAnimation;
    std::shared_ptr<Util::Animation> targetAnimation;

    if (m_CurrentVisualStage == 1) {
        sourceAnimation = m_WallnutStage1Animation;
    } else if (m_CurrentVisualStage == 2) {
        sourceAnimation = m_WallnutStage2Animation;
    } else {
        sourceAnimation = m_WallnutStage3Animation;
    }

    if (targetStage == 1) {
        targetAnimation = m_WallnutStage1Animation;
    } else if (targetStage == 2) {
        targetAnimation = m_WallnutStage2Animation;
    } else {
        targetAnimation = m_WallnutStage3Animation;
    }

    if (sourceAnimation && targetAnimation && targetAnimation->GetFrameCount() > 0) {
        const std::size_t syncIndex = sourceAnimation->GetCurrentFrameIndex() % targetAnimation->GetFrameCount();
        targetAnimation->SetCurrentFrame(syncIndex);
        targetAnimation->Play();
    }

    SetDrawable(targetAnimation);
    m_CurrentVisualStage = targetStage;
}
