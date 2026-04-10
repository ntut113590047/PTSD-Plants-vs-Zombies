#include "SunflowerPlant.hpp"

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include "Util/Animation.hpp"

namespace {
std::vector<std::string> BuildBrightFramePaths(const std::vector<std::string>& normalPaths) {
    std::vector<std::string> brightPaths;
    brightPaths.reserve(normalPaths.size());

    for (const auto& normalPath : normalPaths) {
        std::string brightPath = normalPath;
        const std::string from = "/sunflower/";
        const std::string to = "/sunflower_bright/";
        const std::size_t pos = brightPath.find(from);
        if (pos != std::string::npos) {
            brightPath.replace(pos, from.size(), to);
        }
        brightPaths.push_back(brightPath);
    }

    bool allExist = !brightPaths.empty();
    for (const auto& path : brightPaths) {
        if (!std::filesystem::exists(path)) {
            allExist = false;
            break;
        }
    }

    return allExist ? brightPaths : normalPaths;
}
}

SunflowerPlant::SunflowerPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 80, true, 0),
            20.0f,
            300.0f,
            0.0f,
            24.0f) {
    m_NormalAnimation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
    auto brightPaths = BuildBrightFramePaths(data.plantAnimationPaths);
    m_BrightAnimation = std::make_shared<Util::Animation>(brightPaths, true, 80, true, 0);
}

void SunflowerPlant::Update(float deltaTime) {
    m_SunTimer += deltaTime;
    UpdateBrightAnimation(deltaTime);
}

bool SunflowerPlant::TryProduceSun() {
    if (m_SunTimer < m_SunInterval) {
        return false;
    }

    m_SunTimer -= m_SunInterval;
    m_IsShowingBright = false;
    m_BrightReturnTimer = m_PostSunReturnDelay;
    return true;
}

int SunflowerPlant::GetSunProductionAmount() const {
    return m_SunAmount;
}

void SunflowerPlant::UpdateBrightAnimation(float deltaTime) {
    const float rampStart = std::max(0.0f, m_SunInterval - m_PreSunShowBrightDuration);
    bool shouldShowBright = false;

    if (m_BrightReturnTimer > 0.0f) {
        m_BrightReturnTimer -= deltaTime;
        shouldShowBright = true;
    } else if (m_SunTimer > rampStart) {
        shouldShowBright = true;
    }

    if (shouldShowBright != m_IsShowingBright) {
        m_IsShowingBright = shouldShowBright;
        auto targetAnim = shouldShowBright ? m_BrightAnimation : m_NormalAnimation;
        if (targetAnim && m_NormalAnimation && targetAnim->GetFrameCount() > 0) {
            const std::size_t syncIndex = m_NormalAnimation->GetCurrentFrameIndex() % targetAnim->GetFrameCount();
            targetAnim->SetCurrentFrame(syncIndex);
            targetAnim->Play();
        }
        SetDrawable(targetAnim);
    }
}
