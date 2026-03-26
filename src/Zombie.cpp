#include "Zombie.hpp"

#include <algorithm>

#include "Util/Animation.hpp"

namespace {
void SetDrawableKeepingAnimationPhase(const std::shared_ptr<Core::Drawable>& current,
                                      const std::shared_ptr<Core::Drawable>& target,
                                      Zombie* zombie) {
    if (!target || !zombie) {
        return;
    }

    if (current == target) {
        return;
    }

    auto currentAnim = std::dynamic_pointer_cast<Util::Animation>(current);
    auto targetAnim = std::dynamic_pointer_cast<Util::Animation>(target);

    if (currentAnim && targetAnim && targetAnim->GetFrameCount() > 0) {
        const auto sourceIndex = currentAnim->GetCurrentFrameIndex();
        const auto targetIndex = std::min(sourceIndex, targetAnim->GetFrameCount() - 1);
        targetAnim->SetCurrentFrame(targetIndex);
    }

    zombie->SetDrawable(target);
}
} // namespace

Zombie::Zombie(const std::shared_ptr<Core::Drawable>& drawable,
                             float speed,
                             int row,
                             float zIndex,
                             float defaultScale,
                             float health,
                             float attackDamage,
                             float attackCooldown)
        : Util::GameObject(drawable, zIndex),
            m_Speed(speed),
            m_Row(row),
            m_Health(health),
            m_AttackDamage(attackDamage),
            m_AttackCooldown(attackCooldown),
            m_AttackTimer(attackCooldown) {
            m_IdleDrawable = drawable;
            m_AttackDrawable = drawable;
    SetImageScale(defaultScale);
}

void Zombie::Update(float deltaTime) {
    m_Transform.translation.x -= m_Speed * deltaTime;
}

void Zombie::Tick(float deltaTime) {
    if (m_HitFlashTimer > 0.0f) {
        m_HitFlashTimer -= deltaTime;
        if (m_HitFlashTimer <= 0.0f) {
            m_HitFlashTimer = 0.0f;
            RefreshCurrentDrawable();
        }
    }
}

void Zombie::SetAttacking(bool attacking) {
    if (m_IsAttacking == attacking) {
        return;
    }
    m_IsAttacking = attacking;
    RefreshCurrentDrawable();
}

bool Zombie::IsAttacking() const {
    return m_IsAttacking;
}

void Zombie::SetImageScale(float scaleX, float scaleY) {
    m_Transform.scale = {scaleX, scaleY};
}

void Zombie::SetImageScale(float uniformScale) {
    SetImageScale(uniformScale, uniformScale);
}

float Zombie::GetSpeed() const {
    return m_Speed;
}

int Zombie::GetRow() const {
    return m_Row;
}

float Zombie::GetHealth() const {
    return m_Health;
}

float Zombie::GetAttackDamage() const {
    return m_AttackDamage;
}

float Zombie::GetAttackCooldown() const {
    return m_AttackCooldown;
}

void Zombie::TakeDamage(float damage) {
    if (damage <= 0.0f || m_Health <= 0.0f) {
        return;
    }
    m_Health -= damage;
    m_HitFlashTimer = m_HitFlashDuration;
    RefreshCurrentDrawable();
}

bool Zombie::IsDead() const {
    return m_Health <= 0.0f;
}

bool Zombie::TryAttack(float deltaTime) {
    if (m_AttackDamage <= 0.0f) {
        return false;
    }
    m_AttackTimer += deltaTime;
    if (m_AttackTimer >= m_AttackCooldown) {
        m_AttackTimer = 0.0f;
        return true;
    }
    return false;
}

void Zombie::ConfigureVisualDrawables(const std::shared_ptr<Core::Drawable>& idleDrawable,
                                      const std::shared_ptr<Core::Drawable>& attackDrawable,
                                      const std::shared_ptr<Core::Drawable>& hitBrightIdleDrawable,
                                      const std::shared_ptr<Core::Drawable>& hitBrightAttackDrawable) {
    if (idleDrawable) {
        m_IdleDrawable = idleDrawable;
    }
    if (attackDrawable) {
        m_AttackDrawable = attackDrawable;
    } else {
        m_AttackDrawable = m_IdleDrawable;
    }
    m_HitBrightIdleDrawable = hitBrightIdleDrawable;
    if (hitBrightAttackDrawable) {
        m_HitBrightAttackDrawable = hitBrightAttackDrawable;
    } else {
        m_HitBrightAttackDrawable = m_HitBrightIdleDrawable;
    }
    RefreshCurrentDrawable();
}

void Zombie::RefreshCurrentDrawable() {
    if (m_HitFlashTimer > 0.0f) {
        if (m_IsAttacking && m_HitBrightAttackDrawable) {
            SetDrawableKeepingAnimationPhase(m_Drawable, m_HitBrightAttackDrawable, this);
            return;
        }

        if (!m_IsAttacking && m_HitBrightIdleDrawable) {
            SetDrawableKeepingAnimationPhase(m_Drawable, m_HitBrightIdleDrawable, this);
            return;
        }
    }

    if (m_IsAttacking && m_AttackDrawable) {
        SetDrawableKeepingAnimationPhase(m_Drawable, m_AttackDrawable, this);
        return;
    }

    if (m_IdleDrawable) {
        SetDrawableKeepingAnimationPhase(m_Drawable, m_IdleDrawable, this);
    }
}
