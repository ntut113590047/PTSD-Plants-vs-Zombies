#include "ConeZombie.hpp"

#include <string>

#include "Util/Animation.hpp"

ConeZombie::ConeZombie(const std::vector<std::string>& framePaths, int row, float zIndex)
    : BasicZombie(framePaths, row, 30.0f) {
    (void)zIndex;
    m_Health = 280.0f;
    m_AttackDamage = 15.0f;
}
