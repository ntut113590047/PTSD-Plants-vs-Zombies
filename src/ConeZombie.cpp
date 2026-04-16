#include "ConeZombie.hpp"

#include <string>

#include "Util/Animation.hpp"

ConeZombie::ConeZombie(const std::vector<std::string>& framePaths, int row, float zIndex)
    : Zombie(std::make_shared<Util::Animation>(framePaths, true, 90, true, 0),
             23.0f,
             row,
             14.0f,
             1.0f,
             640.0f,
             100.0f,
             1.0f) {
    (void)zIndex;

    // Create idle animation
    auto idleAnim = std::make_shared<Util::Animation>(framePaths, true, 90, true, 0);

    // Create attack animation
    std::vector<std::string> attackPaths;
    attackPaths.reserve(framePaths.size());
    for (const auto& path : framePaths) {
        std::string attackPath = path;
        const std::string marker = "/coneZombie/";
        const std::string replacement = "/coneZombieAttack/";
        const auto pos = attackPath.find(marker);
        if (pos != std::string::npos) {
            attackPath.replace(pos, marker.size(), replacement);
        }
        attackPaths.push_back(attackPath);
    }
    auto attackAnim = std::make_shared<Util::Animation>(attackPaths, true, 90, true, 0);

    auto idleBrightPaths = Zombie::BuildBrightFramePaths(framePaths, "/coneZombie/", "/coneZombie_bright/");
    auto attackBrightPaths = Zombie::BuildBrightFramePaths(attackPaths, "/coneZombieAttack/", "/coneZombieAttack_bright/");
    auto idleBrightAnim = std::make_shared<Util::Animation>(idleBrightPaths, true, 90, true, 0);
    auto attackBrightAnim = std::make_shared<Util::Animation>(attackBrightPaths, true, 90, true, 0);

    // Configure drawables: idle and attack
    ConfigureVisualDrawables(idleAnim, attackAnim, idleBrightAnim, attackBrightAnim);
}

