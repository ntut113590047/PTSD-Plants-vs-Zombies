#ifndef BASIC_ZOMBIE_HPP
#define BASIC_ZOMBIE_HPP

#include <string>
#include <vector>

#include "Zombie.hpp"

class BasicZombie : public Zombie {
public:
    BasicZombie(const std::vector<std::string>& framePaths, int row, float speed = 35.0f);
};

#endif