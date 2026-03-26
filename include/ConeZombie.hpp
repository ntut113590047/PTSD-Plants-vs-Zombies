#ifndef CONE_ZOMBIE_HPP
#define CONE_ZOMBIE_HPP

#include "Zombie.hpp"

class ConeZombie : public Zombie {
public:
    ConeZombie(const std::vector<std::string>& framePaths, int row, float zIndex = 2.0f);
};

#endif
