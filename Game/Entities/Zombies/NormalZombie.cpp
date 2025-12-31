#include "../../../Engine/Components/Renderer.h"
#include "../../../Engine/GameStatic.h"
#include "NormalZombie.h"

NormalZombie::NormalZombie() {
    name = normalZombie;
    anim->addAnimation("walk", "res/animations/zombies/normal/walk.png", 5, 10, 46, 0.12);
    anim->addAnimation("eat", "res/animations/zombies/normal/eat.png", 4, 10, 39, 0.06);

    // 死亡动画：资源在 zombies 目录下（与原项目一致：10x1，共 10 帧）。
    anim->addAnimation("die", "res/animations/zombies/die.png", 10, 1, 10, 0.1, false);
    anim->addAnimation("die_1", "res/animations/zombies/die_1.png", 10, 1, 10, 0.2, false);
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
