#include "FootballZombie.hpp"

#include <string>

#include "Util/Animation.hpp"

FootballZombie::FootballZombie(const std::vector<std::string>& framePaths, int row, float zIndex)
    : BasicZombie(framePaths, row, 50.0f) {
    (void)zIndex;
    m_Health = 400.0f;
    m_AttackDamage = 20.0f;
}
