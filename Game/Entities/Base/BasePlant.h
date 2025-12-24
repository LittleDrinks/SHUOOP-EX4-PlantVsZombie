#pragma once
#include "../../../Engine/Components/Animator.h"
#include "../../../Engine/Components/BoxCollider.h"
#include "../../../Engine/Components/Renderer.h"
#include "../../../Engine/GameStatic.h"
#include "../../../Engine/coreMinimal.h"


// BasePlant：植物对象基类。
// 职责：
// - 作为 StaticMesh，默认具备 SpriteRenderer 渲染能力。
// - 持有碰撞体 box 与动画控制器 anim（指针由派生类/构造函数创建并 attach）。
//
// 约束：
// - box/anim 的创建与销毁策略必须在实现文件中保持一致。
// - update() 会被主循环每帧调用。
class BasePlant : public StaticMesh {
public:
    // 植物类型：用于区分派生类与资源/逻辑分支。
    enum PlantName { Default, PeaShooter };

protected:
    // 生命值（单位：点）。hp==0 时表示应进入销毁流程。
    int hp;
    // 花费（单位：阳光点数）。用于 UI/建造逻辑。
    int cost;
    // 碰撞体（AABB）。用于与僵尸/子弹交互。
    BoxCollider* box;
    // 动画控制器。负责把当前帧写入 Object::aniSource。
    Animator* anim;

public:
    // 构造：负责初始化渲染器/动画/碰撞体的默认状态。
    // 约束：构造结束后应满足“可以被渲染/可参与碰撞（若需要）”的最小条件。
    BasePlant();

    // 每帧更新：植物逻辑入口。
    // 调用时机：主循环每帧调用一次。
    // 约束：不要在这里直接做屏幕绘制；渲染应由引擎渲染阶段统一完成。
    virtual void update() override;

    // 受击时的函数
    void takeDamage(int harm);
};