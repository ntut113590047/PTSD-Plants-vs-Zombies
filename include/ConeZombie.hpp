#ifndef CONE_ZOMBIE_HPP
#define CONE_ZOMBIE_HPP

#include "BasicZombie.hpp"

class ConeZombie : public BasicZombie {
public:
    ConeZombie(const std::vector<std::string>& framePaths, int row, float zIndex = 2.0f);
};

#endif
