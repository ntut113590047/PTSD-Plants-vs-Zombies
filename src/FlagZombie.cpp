#include "FlagZombie.hpp"

#include <string>

#include "Util/Animation.hpp"

FlagZombie::FlagZombie(const std::vector<std::string>& framePaths, int row, float zIndex)
    : Zombie(std::make_shared<Util::Animation>(framePaths, true, 90, true, 0),
             28.0f,
             row,
             14.0f,
             1.0f,
             270.0f,
             100.0f,
             1.0f) {
    (void)zIndex;

    auto idleAnim = std::make_shared<Util::Animation>(framePaths, true, 90, true, 0);

    // Create attack animation
    std::vector<std::string> attackPaths;
    attackPaths.reserve(framePaths.size());
    for (const auto& path : framePaths) {
        std::string attackPath = path;
        const std::string marker = "/flagZombie/";
        const std::string replacement = "/flagZombieAttack/";
        const auto pos = attackPath.find(marker);
        if (pos != std::string::npos) {
            attackPath.replace(pos, marker.size(), replacement);
        }
        attackPaths.push_back(attackPath);
    }
    auto attackAnim = std::make_shared<Util::Animation>(attackPaths, true, 90, true, 0);

    // Configure drawables: idle and attack
    // When hit, keep showing idle/attack (no bright variants available)
    ConfigureVisualDrawables(idleAnim, attackAnim, idleAnim, attackAnim);
}
