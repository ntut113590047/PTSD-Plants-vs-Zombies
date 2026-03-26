#ifndef FLAG_ZOMBIE_HPP
#define FLAG_ZOMBIE_HPP

#include "Zombie.hpp"

class FlagZombie : public Zombie {
public:
    FlagZombie(const std::vector<std::string>& framePaths, int row, float zIndex = 2.0f);
};

#endif
