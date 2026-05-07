#include "CherryBombPlant.hpp"

#include <algorithm>
#include <cmath>

#include "Util/Animation.hpp"
#include "Zombie.hpp"

CherryBombPlant::CherryBombPlant(const PlantData& data)
    : Plant(data,
            std::make_shared<Util::Animation>(data.plantAnimationPaths, true, 50, false, 0),
            20.0f,
            300.0f,
            1800.0f,
            0.1f) {}

void CherryBombPlant::Update(float deltaTime) {
    m_TimeSincePlanted += deltaTime;
}

std::optional<PlantProjectile> CherryBombPlant::Attack(const std::vector<std::shared_ptr<Zombie>>& zombies, float deltaTime) {
    (void)deltaTime;

    std::shared_ptr<Util::Animation> anim = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);

    bool readyToExplode = false;
    if (anim) {
        const std::size_t frameCount = anim->GetFrameCount();
        if (frameCount > 0) {
            const std::size_t triggerFrameIndex = std::min<std::size_t>(11, frameCount - 1); // frame_12 (1-based)
            readyToExplode = anim->GetCurrentFrameIndex() >= triggerFrameIndex;
        }
    } else {
        // Fallback: 12 frames with 50ms interval => around 0.55s before detonation.
        readyToExplode = m_TimeSincePlanted >= 0.55f;
    }

    if (!m_HasExploded && readyToExplode) {
        // 3x3 blast: self column ± 1 column (cellWidth=100), adjacent rows ± 1.
        // Threshold = 1.5 * cellWidth = 150 to fully cover the neighboring column.
        for (const auto& zombie : zombies) {
            if (!zombie || zombie->IsDead()) {
                continue;
            }
            const float dx = std::abs(zombie->m_Transform.translation.x - m_Transform.translation.x);
            const int rowDiff = std::abs(zombie->GetRow() - GetRow());
            if (rowDiff <= 1 && dx <= 150.0f) {
                zombie->TakeDamage(GetAttackDamage());
            }
        }

        m_HasExploded = true;
    }

    if (m_HasExploded && !m_HasScheduledRemoval) {
        bool finished = false;
        if (anim) {
            finished = (anim->GetState() == Util::Animation::State::ENDED);
        } else {
            // Fallback: remove after full 19-frame playback duration.
            finished = m_TimeSincePlanted >= 0.95f;
        }

        if (finished) {
            m_HasScheduledRemoval = true;
            TakeDamage(GetHealth());
        }
    }

    return std::nullopt;
}
