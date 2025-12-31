#pragma once
#include "../../Engine/Components/Renderer.h"

class Background: public StaticMesh {
public:
    Background() {
        if (auto *sr = dynamic_cast<SpriteRenderer*>(renderer)) {
            sr->load("res/images/bg5.jpg");
            sr->setLayer(-1);
        }
    }
    void update() override {}
};