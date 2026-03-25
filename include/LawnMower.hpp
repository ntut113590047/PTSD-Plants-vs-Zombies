#ifndef LAWN_MOWER_HPP
#define LAWN_MOWER_HPP

#include <memory>

#include "Util/GameObject.hpp"

class LawnMower : public Util::GameObject {
public:
    explicit LawnMower(int row, float zIndex = 12.0f, float defaultScale = 1.0f);

    void SetImageScale(float scaleX, float scaleY);
    void SetImageScale(float uniformScale);

    int GetRow() const;
    bool IsActive() const { return m_Active; }
    void Activate() { m_Active = true; }

    static constexpr float kMoveSpeed = 500.0f;

private:
    int m_Row = 0;
    bool m_Active = false;
};

#endif
