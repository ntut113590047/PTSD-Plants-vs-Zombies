#include "PoleVaulterZombie.hpp"

#include <string>

#include "Util/Animation.hpp"

PoleVaulterZombie::PoleVaulterZombie(const std::vector<std::string>& framePaths, int row, float zIndex)
    : BasicZombie(framePaths, row, 40.0f) {
    (void)zIndex;
    m_Health = 180.0f;
    m_AttackDamage = 15.0f;
}
