#ifndef BUCKET_ZOMBIE_HPP
#define BUCKET_ZOMBIE_HPP

#include "BasicZombie.hpp"

class BucketZombie : public BasicZombie {
public:
    BucketZombie(const std::vector<std::string>& framePaths, int row, float zIndex = 2.0f);
};

#endif
