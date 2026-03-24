#include "PlantCard.hpp"

PlantCard::PlantCard(const PlantData& data, float x, float y)
    : Util::GameObject(std::make_shared<Util::Image>(data.cardImagePath), 10), m_Data(data)
{
    m_Transform.translation.x = x;
    m_Transform.translation.y = y;
    m_Transform.scale = {0.25f, 0.25f};
}

void PlantCard::Follow(float parentY) {
    m_Transform.translation.y = parentY;
}