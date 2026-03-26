#include "WallnutPlant.hpp"

WallnutPlant::WallnutPlant(const PlantData& data,
                           const std::shared_ptr<Core::Drawable>& drawable,
                           float zIndex,
                           float health)
    : Plant(data, drawable, zIndex, health, 0.0f, 0.0f) {}

bool WallnutPlant::DetectZombieInFront(const std::vector<std::shared_ptr<Zombie>>& zombies) const {
    // Walnut doesn't attack
    return false;
}

std::optional<PlantProjectile> WallnutPlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    // Walnut doesn't attack, only provides defense
    return std::nullopt;
}
