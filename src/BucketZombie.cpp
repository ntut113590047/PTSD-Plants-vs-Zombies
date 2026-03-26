#include "BucketZombie.hpp"

#include <string>

#include "Util/Animation.hpp"

BucketZombie::BucketZombie(const std::vector<std::string>& framePaths, int row, float zIndex)
    : BasicZombie(framePaths, row, 28.0f) {
    (void)zIndex;
    m_Health = 350.0f;
    m_AttackDamage = 15.0f;
}
