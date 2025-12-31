#include "SunFlower.h"

SunFlower::SunFlower() {
    hp = 8;
    cost = 50;

    anim->addAnimation("idle", "res/animations/plants/sunflower.png", 4, 6, 24, 0.06);
    anim->addAnimation("produce", "res/animations/plants/sunflower_1.png", 4, 6, 24, 0.06);
    anim->play("idle");

    produceTimer.bind(produceInterval, &SunFlower::startProduce, this, true);
}

void SunFlower::startProduce() {
    anim->play("produce");

    Sun* sun = GameStatic::createObject<Sun>(getWorldPosition() + Point(-10, -20));
    if (sun) {
        sun->setLandingY(getWorldPosition().y + 40);
    }

    revertTimer.bind(1.2, &SunFlower::finishProduce, this, false);
}

void SunFlower::finishProduce() {
    anim->play("idle");
}

void SunFlower::update() {
    BasePlant::update();

    if (enabled) {
        produceTimer.tick();
        revertTimer.tick();
    }
}
