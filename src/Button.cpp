#include "Button.hpp"

Button::Button(const std::string& normalPath, const std::string& hoverPath)
    : Util::GameObject(nullptr, 5)  // 先傳空，之後再設
{
    m_NormalImage = std::make_shared<Util::Image>(normalPath);
    m_HoverImage  = std::make_shared<Util::Image>(hoverPath);

    // 預設顯示正常圖
    SetDrawable(m_NormalImage);
}


void Button::SetPosition(float x, float y){
    m_Transform.translation = {x, y};
}
void Button::SetImage(const std::string& ImagePath) {
    m_ImagePath = ImagePath;

    m_Drawable = std::make_shared<Util::Image>(m_ImagePath);
}
void Button::SetScale(float scaleX, float scaleY){
	m_Transform.scale = {scaleX, scaleY};
}
void Button::UpdateHoverState(float mouseX, float mouseY) {
    // 簡單 AABB 檢測（假設 pivot 為中心）
    auto size = m_NormalImage->GetSize();           // 或用 m_HoverImage 也行
    glm::vec2 scaledSize = size * m_Transform.scale;

    float left   = m_Transform.translation.x - scaledSize.x * 0.5f;
    float right  = m_Transform.translation.x + scaledSize.x * 0.5f;
    float bottom = m_Transform.translation.y - scaledSize.y * 0.5f;
    float top    = m_Transform.translation.y + scaledSize.y * 0.5f;

    bool nowHovered = (mouseX >= left && mouseX <= right &&
                       mouseY >= bottom && mouseY <= top);

    if (nowHovered != m_IsHovered) {
        m_IsHovered = nowHovered;
        if (m_IsHovered) {
            SetDrawable(m_HoverImage);
        } else {
            SetDrawable(m_NormalImage);
        }
    }
}
bool Button::IsClicked() {
    return m_IsHovered && Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB);
}