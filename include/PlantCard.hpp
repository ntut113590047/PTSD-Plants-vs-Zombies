#ifndef PLANTCARD_HPP
#define PLANTCARD_HPP

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include <string>

struct PlantData {
    std::string name;
    int cost;
    float cooldown;
    std::string cardImagePath;
};
class PlantCard : public Util::GameObject {
public:
    PlantCard(const PlantData& data, float x, float y);

    void Follow(float parentY);

private:
    PlantData m_Data;
};

#endif