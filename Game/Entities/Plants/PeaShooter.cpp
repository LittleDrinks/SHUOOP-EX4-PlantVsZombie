#include <iostream>

#include "PeaShooter.h"

PeaShooter::PeaShooter() {
    hp = 10;
    cost = 100;
    anim->addAnimation("idle", "../res/animations/plants/peashooter.png", 4, 6,
                       24, 0.05);
    tm.bind(shootInterval, &PeaShooter::shoot, this, true);
}

void PeaShooter::update() {
    BasePlant::update();
    tm.tick();
}

void PeaShooter::shoot() {
    // 计算生成位置
    Point spawnPos = getWorldPosition() + Point(70, 0);

    // 生成子弹
    GameStatic::createObject<Pea>(spawnPos);
}