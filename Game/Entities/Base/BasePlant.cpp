#include "BasePlant.h"

BasePlant::BasePlant() {
    // 创建并挂载动画加载器
    anim = GameStatic::createComponent<Animator>(Point(0, 0));
    anim->setAttachment(root);

    // 创建并挂载碰撞体（用于被僵尸“碰到并吃掉”等交互）
    box = GameStatic::createComponent<BoxCollider>(Point(15, 10));
    box->setAttachment(root);
    box->setType("Plant");
    box->setSize(Point(50, 60));

    // 基础属性默认值（派生类可覆盖）
    hp = 1;
    cost = 0;
}

void BasePlant::update() {
    if (hp <= 0) {
        Destroy();
    }
}

void BasePlant::takeDamage(int harm) {
    std::cerr << "Plant took " << harm << " damage." << std::endl;
    hp -= harm;
    if (hp <= 0) {
        Destroy();
    }
}