#include <vector>

#include "BaseZombie.h"

BaseZombie::BaseZombie() {
    // 创建并挂载动画加载器
    anim = GameStatic::createComponent<Animator>(Point(0, 0));
    anim->setAttachment(root);

    // 创建并挂载碰撞体（用于与植物/子弹交互）
    box = GameStatic::createComponent<BoxCollider>(Point(20, 30));
    box->setAttachment(root);
    box->setType("Zombie");
    box->setSize(Point(35, 85));

    // 配置吃植物计时器
    eatTimer.bind(0.6, &BaseZombie::eat, this, true);

    // ash.Load("../res/animations/zombies/ash.png", 1, 20, 20, 0.1);
}

BaseZombie::~BaseZombie() {
    if (renderer) {
        renderer->Destruct();
    }
    if (box) {
        box->Destruct();
    }
}

void BaseZombie::update() {
    // 说明：碰撞集合由 CollisionJudge() 在上一帧写入。
    // 本帧 update() 在 CollisionJudge()
    // 之前执行，因此读取到的是“上一帧碰撞结果”。
    judge();

    // 0: 走路 1: 吃植物 2: 死亡
    if (state == 0) {
        // slowFlag: 2=正常速度，1=减速一半
        addPosition(speed * (slowFlag / 2.0));
        anim->play("walk");
    } else if (state == 1) {
        anim->play("eat");
        eatTimer.tick();
    } else {
        // TODO: 播放死亡动画/灰烬效果后再销毁。
        Destroy();
    }
}

void BaseZombie::judge() {
    if (box) {
        std::vector<Object*> buf = box->getCollisions("Plant");
        collisions.clear();

        if (!buf.empty()) {
            for (auto &item: buf) {
                if (BasePlant *p = Cast<BasePlant>(item)) {
                    // TODO
                    collisions.push_back(p);
                }
            }
        }

        if (!collisions.empty()) {
            if (state == 0) {
                state = 1;
                eatTimer.reset();

            }
        } else if (state == 1) {
            state = 0;
            // TODO: Walk
        }
    }
}

void BaseZombie::eat() {
    const auto& plants = box->getCollisions("Plant");
    if (!plants.empty()) {
        if (auto* plant = Cast<BasePlant>(plants[0])) {
            plant->takeDamage(1);
        }
    }
}

void BaseZombie::getAttack(int harm) {
    hp -= harm;
    if (hp <= 0) {
        state = 2;  // 死亡状态
        // anim->play("die");
    }
}