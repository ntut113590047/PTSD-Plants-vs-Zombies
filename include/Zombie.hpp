#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include <memory>
#include <vector>

#include "Util/GameObject.hpp"

class Zombie : public Util::GameObject {
public:
    Zombie(const std::shared_ptr<Core::Drawable>& drawable,
           float speed,
           int row,
           float zIndex = 14.0f,
           float defaultScale = 1.0f,
           float health = 200.0f,
           float attackDamage = 15.0f,
           float attackCooldown = 1.0f);

    virtual ~Zombie() = default;

    virtual void Update(float deltaTime);
    void Tick(float deltaTime);
    void SetAttacking(bool attacking);
    bool IsAttacking() const;
    void SetImageScale(float scaleX, float scaleY);
    void SetImageScale(float uniformScale);

    float GetSpeed() const;
    int GetRow() const;
    float GetHealth() const;
    float GetAttackDamage() const;
    float GetAttackCooldown() const;
    void TakeDamage(float damage);
    void ApplySlow(float slowMultiplier, float duration);
    bool IsDead() const;
    bool TryAttack(float deltaTime);

protected:
    void ConfigureVisualDrawables(const std::shared_ptr<Core::Drawable>& idleDrawable,
                                  const std::shared_ptr<Core::Drawable>& attackDrawable,
                                  const std::shared_ptr<Core::Drawable>& hitBrightIdleDrawable = nullptr,
                                  const std::shared_ptr<Core::Drawable>& hitBrightAttackDrawable = nullptr);

private:
    void RefreshCurrentDrawable();

protected:
    float m_Speed = 35.0f;
    int m_Row = 0;
    float m_Health = 200.0f;
    float m_AttackDamage = 15.0f;
    float m_AttackCooldown = 1.0f;
    float m_AttackTimer = 0.0f;
    bool m_IsAttacking = false;
    float m_SlowMultiplier = 1.0f;
    float m_SlowTimer = 0.0f;
    float m_HitFlashTimer = 0.0f;
    float m_HitFlashDuration = 0.10f;
    std::shared_ptr<Core::Drawable> m_IdleDrawable = nullptr;
    std::shared_ptr<Core::Drawable> m_AttackDrawable = nullptr;
    std::shared_ptr<Core::Drawable> m_HitBrightIdleDrawable = nullptr;
    std::shared_ptr<Core::Drawable> m_HitBrightAttackDrawable = nullptr;
};

#endif