#ifndef BUCKET_ZOMBIE_HPP
#define BUCKET_ZOMBIE_HPP

#include "Zombie.hpp"

class BucketZombie : public Zombie {
public:
    BucketZombie(const std::vector<std::string>& framePaths, int row, float zIndex = 2.0f);
};

#endif
