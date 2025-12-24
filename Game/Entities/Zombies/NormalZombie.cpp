#include "../../../Engine/Components/Renderer.h"
#include "../../../Engine/GameStatic.h"
#include "NormalZombie.h"

NormalZombie::NormalZombie() {
    hp = 2;  // for test

    name = normalZombie;
    anim->addAnimation("walk", "../res/animations/zombies/normal/walk.png", 5, 10, 46, 0.05);
    anim->play("walk");
}

void NormalZombie::judge() {
    // 1) 死亡优先
    if (hp <= 0) {
        state = 2;
        return;
    }

    // 2) 查询上一帧碰撞结果：是否碰到植物
    // 规则：只要当前帧“触碰到任意 Plant”，就切到吃植物状态。
    const auto& plants = box->getCollisions("Plant");
    if (!plants.empty()) {
        state = 1;
        return;
    }

    // 3) 默认走路
    state = 0;
}
