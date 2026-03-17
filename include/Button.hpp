#ifndef PTSD_PLANTS_VS_ZOMBIES_BUTTON_HPP
#define PTSD_PLANTS_VS_ZOMBIES_BUTTON_HPP
#include <string>
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
class Button : public Util::GameObject {
private:
    std::string m_ImagePath;
    std::shared_ptr<Util::Image> m_NormalImage;
    std::shared_ptr<Util::Image> m_HoverImage;
    bool m_IsHovered = false;

public:
    Button(const std::string& normalPath, const std::string& hoverPath);

    void SetPosition(float x, float y);
    void SetScale(float sx, float sy);

    void SetImage(const std::string& ImagePath);
    void UpdateHoverState(float mouseX, float mouseY);
    bool IsHovered() const { return m_IsHovered; }
    bool IsClicked();
};
#endif //PTSD_PLANTS_VS_ZOMBIES_BUTTON_HPP