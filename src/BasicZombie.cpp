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

    std::vector<std::string> idleBrightPaths;
    idleBrightPaths.reserve(framePaths.size());
    for (const auto& path : framePaths) {
        std::string brightPath = path;
        const std::string marker = "/zombie/";
        const std::string replacement = "/zombie_bright/";
        const auto pos = brightPath.find(marker);
        if (pos != std::string::npos) {
            brightPath.replace(pos, marker.size(), replacement);
        }
        idleBrightPaths.push_back(brightPath);
    }
    auto idleBrightAnim = std::make_shared<Util::Animation>(idleBrightPaths, true, 90, true, 0);

    std::vector<std::string> attackPaths;
    std::vector<std::string> attackBrightPaths;
    for (int i = 1; i <= 40; ++i) {
        attackPaths.push_back(RESOURCE_DIR"/Image/zombies/zombieAttack/frame_" + std::to_string(i) + ".png");
        attackBrightPaths.push_back(RESOURCE_DIR"/Image/zombies/zombieAttack_bright/frame_" + std::to_string(i) + ".png");
    }
    auto attackAnim = std::make_shared<Util::Animation>(attackPaths, true, 80, true, 0);
    auto attackBrightAnim = std::make_shared<Util::Animation>(attackBrightPaths, true, 80, true, 0);

    ConfigureVisualDrawables(idleAnim, attackAnim, idleBrightAnim, attackBrightAnim);
}
