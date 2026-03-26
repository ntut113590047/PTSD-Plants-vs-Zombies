#include "BucketZombie.hpp"

#include <string>

#include "Util/Animation.hpp"

BucketZombie::BucketZombie(const std::vector<std::string>& framePaths, int row, float zIndex)
    : Zombie(std::make_shared<Util::Animation>(framePaths, true, 90, true, 0),
             28.0f,
             row,
             14.0f,
             1.0f,
             350.0f,
             15.0f,
             1.0f) {
    (void)zIndex;

    auto idleAnim = std::make_shared<Util::Animation>(framePaths, true, 90, true, 0);

    // Create bright idle frames
    std::vector<std::string> idleBrightPaths;
    idleBrightPaths.reserve(framePaths.size());
    for (const auto& path : framePaths) {
        std::string brightPath = path;
        const std::string marker = "/bucket/";
        const std::string replacement = "/bucket_bright/";
        const auto pos = brightPath.find(marker);
        if (pos != std::string::npos) {
            brightPath.replace(pos, marker.size(), replacement);
        }
        idleBrightPaths.push_back(brightPath);
    }
    auto idleBrightAnim = std::make_shared<Util::Animation>(idleBrightPaths, true, 90, true, 0);

    ConfigureVisualDrawables(idleAnim, idleAnim, idleBrightAnim, idleBrightAnim);
}
