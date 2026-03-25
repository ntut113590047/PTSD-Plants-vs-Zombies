#include "LawnMower.hpp"

#include "Util/Image.hpp"

LawnMower::LawnMower(int row, float zIndex, float defaultScale)
    : Util::GameObject(std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Other/lawnMower.png"), zIndex),
      m_Row(row) {
    SetImageScale(defaultScale);
}

void LawnMower::SetImageScale(float scaleX, float scaleY) {
    m_Transform.scale = {scaleX, scaleY};
}

void LawnMower::SetImageScale(float uniformScale) {
    SetImageScale(uniformScale, uniformScale);
}

int LawnMower::GetRow() const {
    return m_Row;
}
