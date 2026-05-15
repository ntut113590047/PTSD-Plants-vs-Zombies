#include "Zombie.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>

#include "Util/Animation.hpp"

namespace fs = std::filesystem;

namespace {
constexpr float kZombieHealthScale = 0.75f;
}

std::vector<std::string> Zombie::BuildBrightFramePaths(const std::vector<std::string>& normalPaths,
                                                           const std::string& normalMarker,
                                                           const std::string& brightMarker) {
    std::vector<std::string> brightPaths;
    brightPaths.reserve(normalPaths.size());
    for (const auto& path : normalPaths) {
        std::string brightPath = path;
        const auto pos = brightPath.find(normalMarker);
        if (pos == std::string::npos) {
            return normalPaths;
        }
        brightPath.replace(pos, normalMarker.size(), brightMarker);
        if (!fs::exists(brightPath)) {
            return normalPaths;
        }
        brightPaths.push_back(brightPath);
    }
    return brightPaths;
}

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
            m_Health(health * kZombieHealthScale),
            m_AttackDamage(attackDamage),
            m_AttackCooldown(attackCooldown),
            m_AttackTimer(attackCooldown) {
            m_IdleDrawable = drawable;
            m_AttackDrawable = drawable;
            m_SlowIdleDrawable = drawable;
            m_SlowAttackDrawable = drawable;
    SetImageScale(defaultScale);
}

void Zombie::Update(float deltaTime) {
    const float effectiveSpeed = m_Speed * GetEffectiveSlowMultiplier();
    m_Transform.translation.x -= effectiveSpeed * deltaTime;
}

void Zombie::Tick(float deltaTime) {
    UpdateSlowState(deltaTime);
    UpdateAnimationSpeeds();

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

void Zombie::ApplySlow(float slowMultiplier, float duration) {
    if (duration <= 0.0f) {
        return;
    }

    const float clampedMultiplier = std::clamp(slowMultiplier, 0.05f, 1.0f);
    m_SlowMultiplier = clampedMultiplier;
    m_SlowTimer = duration;
    UpdateAnimationSpeeds();
    RefreshCurrentDrawable();
}

bool Zombie::IsDead() const {
    return m_Health <= 0.0f;
}

bool Zombie::TryAttack(float deltaTime) {
    if (m_AttackDamage <= 0.0f) {
        return false;
    }
    m_AttackTimer += deltaTime * GetEffectiveSlowMultiplier();
    if (m_AttackTimer >= m_AttackCooldown) {
        m_AttackTimer = 0.0f;
        return true;
    }
    return false;
}

void Zombie::UpdateSlowState(float deltaTime) {
    if (m_SlowTimer <= 0.0f) {
        return;
    }

    m_SlowTimer -= deltaTime;
    if (m_SlowTimer <= 0.0f) {
        m_SlowTimer = 0.0f;
        m_SlowMultiplier = 1.0f;
        UpdateAnimationSpeeds();
        RefreshCurrentDrawable();
    }
}

float Zombie::GetEffectiveSlowMultiplier() const {
    if (m_SlowTimer > 0.0f && m_SlowMultiplier < 1.0f) {
        return m_SlowMultiplier;
    }
    return 1.0f;
}

void Zombie::UpdateAnimationSpeeds() {
    const float slowMultiplier = GetEffectiveSlowMultiplier();

    UpdateAnimationSpeedForDrawable(m_IdleDrawable, slowMultiplier);
    UpdateAnimationSpeedForDrawable(m_AttackDrawable, slowMultiplier);
    UpdateAnimationSpeedForDrawable(m_HitBrightIdleDrawable, slowMultiplier);
    UpdateAnimationSpeedForDrawable(m_HitBrightAttackDrawable, slowMultiplier);
    UpdateAnimationSpeedForDrawable(m_SlowIdleDrawable, slowMultiplier);
    UpdateAnimationSpeedForDrawable(m_SlowAttackDrawable, slowMultiplier);
}

void Zombie::UpdateAnimationSpeedForDrawable(const std::shared_ptr<Core::Drawable>& drawable, float slowMultiplier) {
    if (!drawable) {
        return;
    }

    auto anim = std::dynamic_pointer_cast<Util::Animation>(drawable);
    if (!anim) {
        return;
    }

    const Core::Drawable* key = drawable.get();
    auto it = m_BaseAnimationIntervals.find(key);
    if (it == m_BaseAnimationIntervals.end()) {
        m_BaseAnimationIntervals[key] = anim->GetInterval();
        it = m_BaseAnimationIntervals.find(key);
    }

    const int baseInterval = std::max(1, it->second);
    const float safeMultiplier = std::max(0.05f, slowMultiplier);
    const int slowedInterval = static_cast<int>(std::round(static_cast<float>(baseInterval) / safeMultiplier));
    anim->SetInterval(std::max(1, slowedInterval));
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

    if (!m_SlowIdleDrawable) {
        m_SlowIdleDrawable = m_IdleDrawable;
    }
    if (!m_SlowAttackDrawable) {
        m_SlowAttackDrawable = m_AttackDrawable;
    }

    RefreshCurrentDrawable();
}

void Zombie::ConfigureSlowVisualDrawables(const std::shared_ptr<Core::Drawable>& slowIdleDrawable,
                                          const std::shared_ptr<Core::Drawable>& slowAttackDrawable) {
    if (slowIdleDrawable) {
        m_SlowIdleDrawable = slowIdleDrawable;
    } else {
        m_SlowIdleDrawable = m_IdleDrawable;
    }

    if (slowAttackDrawable) {
        m_SlowAttackDrawable = slowAttackDrawable;
    } else {
        m_SlowAttackDrawable = m_SlowIdleDrawable ? m_SlowIdleDrawable : m_AttackDrawable;
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

    const bool isSlowed = (m_SlowTimer > 0.0f && m_SlowMultiplier < 1.0f);
    if (isSlowed) {
        if (m_IsAttacking && m_SlowAttackDrawable) {
            SetDrawableKeepingAnimationPhase(m_Drawable, m_SlowAttackDrawable, this);
            return;
        }

        if (!m_IsAttacking && m_SlowIdleDrawable) {
            SetDrawableKeepingAnimationPhase(m_Drawable, m_SlowIdleDrawable, this);
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
