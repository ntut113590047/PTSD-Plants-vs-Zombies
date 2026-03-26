#include "ConeZombie.hpp"

#include <string>

#include "Util/Animation.hpp"

ConeZombie::ConeZombie(const std::vector<std::string>& framePaths, int row, float zIndex)
    : Zombie(std::make_shared<Util::Animation>(framePaths, true, 90, true, 0),
             30.0f,
             row,
             14.0f,
             1.0f,
             280.0f,
             15.0f,
             1.0f) {
    (void)zIndex;

    // Create idle animation
    auto idleAnim = std::make_shared<Util::Animation>(framePaths, true, 90, true, 0);

    // Create bright idle frames (hit effect)
    std::vector<std::string> idleBrightPaths;
    idleBrightPaths.reserve(framePaths.size());
    for (const auto& path : framePaths) {
        std::string brightPath = path;
        const std::string marker = "/cone/";
        const std::string replacement = "/cone_bright/";
        const auto pos = brightPath.find(marker);
        if (pos != std::string::npos) {
            brightPath.replace(pos, marker.size(), replacement);
        }
        idleBrightPaths.push_back(brightPath);
    }
    auto idleBrightAnim = std::make_shared<Util::Animation>(idleBrightPaths, true, 90, true, 0);

    // Configure two drawables: normal and bright
    ConfigureVisualDrawables(idleAnim, idleAnim, idleBrightAnim, idleBrightAnim);
}

