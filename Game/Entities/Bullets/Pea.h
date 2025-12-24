#pragma once
#include "../../../Engine/Components/BoxCollider.h"
#include "../../../Engine/Components/Renderer.h"
#include "../../../Engine/GameStatic.h"
#include "../../../Engine/coreMinimal.h"
#include "../Base/BaseBullet.h"
#include "../Base/BaseZombie.h"

// Pea：豌豆子弹。
//
// 职责：
// - 作为 BaseBullet
// 的具体实现，提供直线飞行与命中判定后的处理（例如对僵尸造成伤害并销毁自身）。
// - update() 中根据 speed 推进位置，并依赖 BoxCollider 参与碰撞检测。
class Pea : public BaseBullet {
public:
    // 构造：初始化渲染资源/碰撞体/默认速度等。
    // 约束：构造后应满足“可渲染 + 可参与碰撞”的最小条件。
    Pea();

    // 每帧更新：推进位置，并处理命中/越界等逻辑。
    // 约束：不要在这里直接绘制；渲染由引擎渲染阶段统一完成。
    void update() override;
};