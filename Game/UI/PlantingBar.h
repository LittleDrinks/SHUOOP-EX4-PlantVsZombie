#pragma once

#include "../../Engine/Components/Renderer.h"

class PlantingBar : public StaticMesh {
public:
    PlantingBar() {
        if (auto *sr = dynamic_cast<SpriteRenderer*>(renderer)) {
            sr->load("res/images/bar.png");
            sr->setLayer(100);
        }
    }
    virtual void update() override {}
};

class PlantCardPeaShooter : public StaticMesh {
public:
    PlantCardPeaShooter() {
        if (auto *sr = dynamic_cast<SpriteRenderer*>(renderer)) {
            sr->load("res/images/cards/card1.png");
            sr->setLayer(101);
        }
    }
    virtual void update() override {}
};
