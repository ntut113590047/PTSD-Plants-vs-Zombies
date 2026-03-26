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

    // Create attack animation for hit flash effect
    std::vector<std::string> attackPaths;
    attackPaths.reserve(framePaths.size());
    for (const auto& path : framePaths) {
        std::string attackPath = path;
        const std::string marker = "/bucketZombie/";
        const std::string replacement = "/bucketZombieAttack/";
        const auto pos = attackPath.find(marker);
        if (pos != std::string::npos) {
            attackPath.replace(pos, marker.size(), replacement);
        }
        attackPaths.push_back(attackPath);
    }
    auto attackAnim = std::make_shared<Util::Animation>(attackPaths, true, 90, true, 0);

    ConfigureVisualDrawables(idleAnim, idleAnim, attackAnim, attackAnim);
}
