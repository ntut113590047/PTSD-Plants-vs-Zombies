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

    auto idleBrightPaths = Zombie::BuildBrightFramePaths(framePaths, "/flagZombie/", "/flagZombie_bright/");
    auto attackBrightPaths = Zombie::BuildBrightFramePaths(attackPaths, "/flagZombieAttack/", "/flagZombieAttack_bright/");
    auto idleBrightAnim = std::make_shared<Util::Animation>(idleBrightPaths, true, 90, true, 0);
    auto attackBrightAnim = std::make_shared<Util::Animation>(attackBrightPaths, true, 90, true, 0);

    auto idleBluePaths = Zombie::BuildBrightFramePaths(framePaths, "/flagZombie/", "/flagZombie_blue/");
    auto attackBluePaths = Zombie::BuildBrightFramePaths(attackPaths, "/flagZombieAttack/", "/flagZombieAttack_blue/");
    auto idleBlueAnim = std::make_shared<Util::Animation>(idleBluePaths, true, 90, true, 0);
    auto attackBlueAnim = std::make_shared<Util::Animation>(attackBluePaths, true, 90, true, 0);

    // Configure drawables: idle and attack
    ConfigureVisualDrawables(idleAnim, attackAnim, idleBrightAnim, attackBrightAnim);
    ConfigureSlowVisualDrawables(idleBlueAnim, attackBlueAnim);
}
