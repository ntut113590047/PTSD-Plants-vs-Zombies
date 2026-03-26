#ifndef FOOTBALL_ZOMBIE_HPP
#define FOOTBALL_ZOMBIE_HPP

#include "BasicZombie.hpp"

class FootballZombie : public BasicZombie {
public:
    FootballZombie(const std::vector<std::string>& framePaths, int row, float zIndex = 2.0f);
};

#endif
