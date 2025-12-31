#include "Pea.h"

Pea::Pea() {
    setSpeed(Point(3.0, 0.0));
    renderer->load("res/animations/bullets/pea.png");
    renderer->setLayer(8);

    box->setSize(Point(25, 25));
}

void Pea::update() {
    // 1) 先处理命中（读取上一帧 CollisionJudge() 写入的碰撞结果）
    const auto& zombies = box->getCollisions("Zombie");
    if (!zombies.empty()) {
        if (auto* zombie = Cast<BaseZombie>(zombies[0])) {
            zombie->getAttack(1);
        }
        Destroy();
        return;
    }

    // 2) 再移动
    addPosition(speed);

    if (getWorldPosition().x > 825) {
        Destroy();
    }
}