#include "BasicZombie.hpp"

#include <string>

#include "Util/Animation.hpp"

BasicZombie::BasicZombie(const std::vector<std::string>& framePaths, int row, float speed)
    : Zombie(std::make_shared<Util::Animation>(framePaths, true, 90, true, 0),
             speed,
             row,
             14.0f,
             1.0f,
             270.0f,
             100.0f,
             1.0f) {
    auto idleAnim = std::make_shared<Util::Animation>(framePaths, true, 90, true, 0);
    auto idleBrightAnim = std::make_shared<Util::Animation>(
        Zombie::BuildBrightFramePaths(framePaths, "/zombie/", "/zombie_bright/"),
        true,
        90,
        true,
        0
    );

    std::vector<std::string> attackPaths;
    for (int i = 1; i <= 40; ++i) {
        attackPaths.push_back(RESOURCE_DIR"/Image/zombies/zombieAttack/frame_" + std::to_string(i) + ".png");
    }
    auto attackAnim = std::make_shared<Util::Animation>(attackPaths, true, 80, true, 0);
    auto attackBrightAnim = std::make_shared<Util::Animation>(
        Zombie::BuildBrightFramePaths(attackPaths, "/zombieAttack/", "/zombieAttack_bright/"),
        true,
        80,
        true,
        0
    );

    auto idleBlueAnim = std::make_shared<Util::Animation>(
        Zombie::BuildBrightFramePaths(framePaths, "/zombie/", "/zombie_blue/"),
        true,
        90,
        true,
        0
    );
    auto attackBlueAnim = std::make_shared<Util::Animation>(
        Zombie::BuildBrightFramePaths(attackPaths, "/zombieAttack/", "/zombieAttack_blue/"),
        true,
        80,
        true,
        0
    );

    ConfigureVisualDrawables(idleAnim, attackAnim, idleBrightAnim, attackBrightAnim);
    ConfigureSlowVisualDrawables(idleBlueAnim, attackBlueAnim);
}
