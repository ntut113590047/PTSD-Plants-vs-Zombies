#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Background : public Util::GameObject {
public:
    explicit Background(const std::string& path)
        : GameObject(std::make_shared<Util::Image>(path), -10) {}
};

#endif