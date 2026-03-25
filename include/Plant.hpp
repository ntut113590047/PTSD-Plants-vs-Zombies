#ifndef PLANT_HPP
#define PLANT_HPP

#include <memory>
#include <optional>
#include <vector>

#include "Util/GameObject.hpp"
#include "PlantCard.hpp"

class Zombie;

struct PlantProjectile {
    std::shared_ptr<Util::GameObject> object;
    int row = 0;
    float speed = 0.0f;
    float damage = 0.0f;
};

class Plant : public Util::GameObject {
public:
    Plant(const PlantData& data,
          const std::shared_ptr<Core::Drawable>& drawable,
          float zIndex = 20.0f,
          float health = 100.0f,
          float attackDamage = 0.0f,
          float attackCooldown = 1.0f);

    virtual ~Plant() = default;

    virtual void Update(float deltaTime);

    const PlantData& GetData() const;

    void SetGridPosition(int row, int col);
    void SetImageScale(float scaleX, float scaleY);
    void SetImageScale(float uniformScale);

    int GetRow() const;
    int GetCol() const;

    float GetHealth() const;
    float GetAttackDamage() const;
    float GetAttackCooldown() const;
    void TakeDamage(float damage);
    bool IsDead() const;
    bool TryAttack(float deltaTime);
    virtual bool DetectZombieInFront(const std::vector<std::shared_ptr<Zombie>>& zombies) const;
    virtual std::optional<PlantProjectile> Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime);

protected:
    PlantData m_Data;
    int m_Row = -1;
    int m_Col = -1;
    float m_Health = 100.0f;
    float m_AttackDamage = 0.0f;
    float m_AttackCooldown = 1.0f;
    float m_AttackTimer = 0.0f;
};

#endif