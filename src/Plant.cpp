#include "Plant.hpp"

#include "Zombie.hpp"

Plant::Plant(const PlantData& data,
                         const std::shared_ptr<Core::Drawable>& drawable,
                         float zIndex,
                         float health,
                         float attackDamage,
                         float attackCooldown)
        : Util::GameObject(drawable, zIndex),
            m_Data(data),
            m_Health(health),
            m_AttackDamage(attackDamage),
            m_AttackCooldown(attackCooldown),
            m_AttackTimer(attackCooldown) {
    SetImageScale(m_Data.scale);
}

void Plant::Update(float deltaTime) {
    (void)deltaTime;
}

const PlantData& Plant::GetData() const {
    return m_Data;
}

void Plant::SetGridPosition(int row, int col) {
    m_Row = row;
    m_Col = col;
}

void Plant::SetImageScale(float scaleX, float scaleY) {
    m_Transform.scale = {scaleX, scaleY};
}

void Plant::SetImageScale(float uniformScale) {
    SetImageScale(uniformScale, uniformScale);
}

int Plant::GetRow() const {
    return m_Row;
}

int Plant::GetCol() const {
    return m_Col;
}

float Plant::GetHealth() const {
    return m_Health;
}

float Plant::GetAttackDamage() const {
    return m_AttackDamage;
}

float Plant::GetAttackCooldown() const {
    return m_AttackCooldown;
}

void Plant::TakeDamage(float damage) {
    if (damage <= 0.0f || m_Health <= 0.0f) {
        return;
    }
    m_Health -= damage;
}

bool Plant::IsDead() const {
    return m_Health <= 0.0f;
}

bool Plant::TryAttack(float deltaTime) {
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

bool Plant::DetectZombieInFront(const std::vector<std::shared_ptr<Zombie>>& zombies) const {
    for (const auto& zombie : zombies) {
        if (zombie->GetRow() != m_Row) {
            continue;
        }
        if (zombie->m_Transform.translation.x >= m_Transform.translation.x) {
            return true;
        }
    }
    return false;
}

std::optional<PlantProjectile> Plant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    (void)zombies;
    (void)deltaTime;
    return std::nullopt;
}
