#ifndef POTATO_MINE_PLANT_HPP
#define POTATO_MINE_PLANT_HPP

#include "Plant.hpp"

namespace Util {
class Image;
class Animation;
}

class PotatoMinePlant : public Plant {
public:
    PotatoMinePlant(const PlantData& data,
                    const std::shared_ptr<Core::Drawable>& drawable = nullptr,
                    float zIndex = 20.0f);

    void Update(float deltaTime) override;
    std::optional<PlantProjectile> Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) override;

private:
    enum class MineState {
        Buried,
        Armed,
        Exploded,
        Finished
    };

    bool IsZombieTouchingMine(const std::shared_ptr<Zombie>& zombie) const;
    void TriggerExplosion(const std::vector<std::shared_ptr<Zombie>>& zombies);

    MineState m_State = MineState::Buried;
    float m_StateTimer = 0.0f;

    std::shared_ptr<Util::Image> m_Potato1Image;
    std::shared_ptr<Util::Animation> m_Potato2Animation;
    std::shared_ptr<Util::Image> m_Potato3Image;
    std::shared_ptr<Util::GameObject> m_BombEffect;

    static constexpr float ARM_DELAY_SECONDS = 14.0f;
    static constexpr float POTATO3_SHOW_SECONDS = 1.0f;
    static constexpr float TRIGGER_X_DISTANCE = 55.0f;
    static constexpr float EXPLOSION_X_RADIUS = 95.0f;
    static constexpr float EXPLOSION_Y_RADIUS = 52.0f;
};

#endif
