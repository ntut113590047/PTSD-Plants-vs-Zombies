#ifndef POLE_VAULTER_ZOMBIE_HPP
#define POLE_VAULTER_ZOMBIE_HPP

#include "Zombie.hpp"

class PoleVaulterZombie : public Zombie {
public:
    PoleVaulterZombie(const std::vector<std::string>& framePaths, int row, float zIndex = 2.0f);
};

#endif
