#ifndef PTSD_PLANTS_VS_ZOMBIES_LEVELMANAGER_H
#define PTSD_PLANTS_VS_ZOMBIES_LEVELMANAGER_H

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"
#include "Button.hpp"
#include "Background.hpp"
#include <memory>
#include <vector>

class LevelManager {
public:
    explicit LevelManager(int level);

    void LoadLevel(Util::Renderer& root);
    void Update(Util::Renderer& root);
    void ChangeLevel(int level, Util::Renderer& root);

private:
    int m_CurrentLevel = 0;

    std::shared_ptr<Util::GameObject> m_Background;
    std::vector<std::shared_ptr<Button>> m_Buttons;
};

#endif