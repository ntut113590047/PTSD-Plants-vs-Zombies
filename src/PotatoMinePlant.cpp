#include "PotatoMinePlant.hpp"

#include <cmath>
#include <string>
#include <vector>

#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Zombie.hpp"

PotatoMinePlant::PotatoMinePlant(const PlantData& data,
                                 const std::shared_ptr<Core::Drawable>& drawable,
                                 float zIndex)
    : Plant(data,
            drawable != nullptr ? drawable : std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Plants/potato1.png"),
            zIndex,
            300.0f,
            1800.0f,
            0.0f) {
    m_Potato1Image = std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Plants/potato1.png");

    std::vector<std::string> potato2Frames;
    for (int i = 1; i <= 8; ++i) {
        potato2Frames.push_back(RESOURCE_DIR"/Image/Plants/potato2/frame_" + std::to_string(i) + ".png");
    }
    m_Potato2Animation = std::make_shared<Util::Animation>(potato2Frames, true, 80, true, 0);

    m_Potato3Image = std::make_shared<Util::Image>(RESOURCE_DIR"/Image/Plants/potato3.png");
    SetDrawable(m_Potato1Image);
}

void PotatoMinePlant::Update(float deltaTime) {
    if (m_State == MineState::Finished) {
        return;
    }

    m_StateTimer += deltaTime;

    if (m_State == MineState::Buried && m_StateTimer >= ARM_DELAY_SECONDS) {
        m_State = MineState::Armed;
        m_StateTimer = 0.0f;
        if (m_Potato2Animation) {
            m_Potato2Animation->SetCurrentFrame(0);
            m_Potato2Animation->Play();
            SetDrawable(m_Potato2Animation);
        }
    } else if (m_State == MineState::Exploded && m_StateTimer >= POTATO3_SHOW_SECONDS) {
        if (m_BombEffect) {
            RemoveChild(m_BombEffect);
            m_BombEffect.reset();
        }
        m_State = MineState::Finished;
        TakeDamage(GetHealth());
    } else if (m_State == MineState::Exploded && m_BombEffect) {
        m_BombEffect->m_Transform.translation = m_Transform.translation;
    }
}

std::optional<PlantProjectile> PotatoMinePlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    (void)deltaTime;
    if (m_State != MineState::Armed) {
        return std::nullopt;
    }

    for (const auto& zombie : zombies) {
        if (IsZombieTouchingMine(zombie)) {
            TriggerExplosion(zombies);
            break;
        }
    }

    return std::nullopt;
}

bool PotatoMinePlant::IsZombieTouchingMine(const std::shared_ptr<Zombie>& zombie) const {
    if (!zombie || zombie->IsDead() || zombie->GetRow() != GetRow()) {
        return false;
    }

    const float dx = std::abs(zombie->m_Transform.translation.x - m_Transform.translation.x);
    return dx <= TRIGGER_X_DISTANCE;
}

void PotatoMinePlant::TriggerExplosion(const std::vector<std::shared_ptr<Zombie>>& zombies) {
    for (const auto& zombie : zombies) {
        if (!zombie || zombie->IsDead() || zombie->GetRow() != GetRow()) {
            continue;
        }

        const float dx = std::abs(zombie->m_Transform.translation.x - m_Transform.translation.x);
        const float dy = std::abs(zombie->m_Transform.translation.y - m_Transform.translation.y);
        if (dx <= EXPLOSION_X_RADIUS && dy <= EXPLOSION_Y_RADIUS) {
            zombie->TakeDamage(GetAttackDamage());
        }
    }

    std::vector<std::string> bombFrames;
    for (int i = 1; i <= 8; ++i) {
        bombFrames.push_back(RESOURCE_DIR"/Image/Other/Bomb/frame_" + std::to_string(i) + ".png");
    }

    auto bombAnimation = std::make_shared<Util::Animation>(bombFrames, true, 50, false, 0);
    m_BombEffect = std::make_shared<Util::GameObject>(bombAnimation, 32.0f);
    m_BombEffect->m_Transform.translation = m_Transform.translation;
    m_BombEffect->m_Transform.scale = {0.55f, 0.55f};
    AddChild(m_BombEffect);

    SetDrawable(m_Potato3Image);
    m_State = MineState::Exploded;
    m_StateTimer = 0.0f;
}
