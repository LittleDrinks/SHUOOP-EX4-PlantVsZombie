#include "BasePlant.h"

BasePlant::BasePlant() {
    // 创建并挂载动画加载器
    anim = GameStatic::createComponent<Animator>(Point(0, 0));
    anim->setAttachment(root);

    // 创建并挂载碰撞体（用于被僵尸“碰到并吃掉”等交互）
    box = GameStatic::createComponent<BoxCollider>(Point(15, 10));
    box->setAttachment(root);
    // 默认未种下：不参与碰撞、也不应被伤害。
    box->setType("PreviewPlant");
    box->setSize(Point(50, 60));
    box->setOpen(false);

    // 基础属性默认值（派生类可覆盖）
    hp = 1;
    cost = 0;
}

void BasePlant::update() {
    // 保持碰撞体开关与 enabled 一致，避免预览/手持植物参与碰撞。
    if (box) {
        box->setOpen(enabled);
        box->setType(enabled ? "Plant" : "PreviewPlant");
    }
    if (hp <= 0) {
        Destroy();
    }
}

void BasePlant::takeDamage(int harm) {
    if (!enabled) {
        return;
    }
    hp -= harm;
    if (hp <= 0) {
        Destroy();
    }
}