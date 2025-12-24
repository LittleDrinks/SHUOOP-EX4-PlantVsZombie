#pragma once
#include <vector>

#include "../../../Engine/Components/Animator.h"
#include "../../../Engine/Components/BoxCollider.h"
#include "../../../Engine/Components/Renderer.h"
#include "../../../Engine/GameStatic.h"
#include "../../../Engine/Overall.h"
#include "../../../Engine/coreMinimal.h"
#include "BasePlant.h"

// BaseZombie：僵尸公共基类（游戏侧）。
//
// 职责：
// - 作为可渲染对象（继承 StaticMesh），承载僵尸的 SpriteRenderer/Animator/BoxCollider 组件。
// - 维护僵尸的基础状态机：走路/吃植物/死亡，并提供被攻击/减速/冻结/恢复等基础行为入口。
// - 记录与植物的接触列表 collisions，用于 judge()/eat() 决策。
//
// 调用约定：
// - update()/judge() 应每帧调用一次（由对象系统或主循环驱动）。
// - BoxCollider 的碰撞回调由引擎 CollisionJudge() 触发；回调内部通常会更新 collisions。
class BaseZombie : public StaticMesh {
public:
    // 僵尸类型枚举：用于区分不同派生僵尸的资源/属性。
    enum zombieName {
        normalZombie,
        hatZombie,
        bucketZombie,
        jumpZombie,
        armorZombie,
        bossZombie,
        flagZombie
    };

protected:
    // 当前僵尸类型。
    zombieName name = normalZombie;

    // 组件：碰撞体。
    // 所有权：由对象/组件系统管理；本类仅保存指针引用。
    BoxCollider* box;

    // 与本僵尸发生接触的植物列表（通常是“可被吃”的目标）。
    // 约束：实现需保证不会重复插入、不会悬空指针（植物销毁时要同步移除或做有效性检查）。
    std::vector<BasePlant*> collisions;

    // 组件：动画控制器。
    Animator* anim;

    // 死亡灰烬/消散等效果动画（具体如何使用由实现决定）。
    Animation ash;

    // 状态机：0=走路，1=吃植物，2=死亡。
    // 约束：派生类可扩展状态，但必须保持这三个值的语义不变。
    int state = 0;  // 0: 走路 1: 吃植物 2: 死亡

    // 当前速度。
    // 单位：与引擎的 DELTA_TIME 语义一致（通常是像素/毫秒或像素/帧）。
    Point speed = Point(-0.06, 0.0);

    // 生命值：降到 0（或更小）应进入死亡流程。
    int hp = 10;

    // 阴影/遮罩渲染器（用于增加层次；如何绘制由实现决定）。
    SpriteRenderer* shade;

    // 减速标记：2=正常速度；1=速度减半。
    // 约束：freeze()/recover() 应维护该标记与 speed/动画速率的一致性。
    double slowFlag = 2;  // 2 正常速度；1 减速一半

public:
    // 精灵表行号（用于 SpriteSheet 裁剪/动画选择）。
    int row = 0;

    // 构造/析构：初始化组件、加载资源、设置初始状态。
    BaseZombie();
    ~BaseZombie();

    // 每帧更新：推进位置/动画，并触发 judge() 进行状态切换。
    // 说明：派生类可 override，但应保持“每帧推进 + 状态判定”的主语义。
    virtual void update();

    // 状态判定：根据 collisions/state/hp 等更新走路/吃/死等状态。
    // 说明：派生类通常 override 以实现差异化 AI。
    virtual void judge();

    // 吃植物：对 collisions 中的目标造成伤害/触发植物死亡等。
    // 约束：需要处理 collisions 为空或目标无效的情况。
    void eat();

    // 受击：降低 hp，并根据结果切换到死亡动画/状态。
    void getAttack(int harm);

    // 恢复：从减速/冻结恢复到正常速度与动画节奏。
    void recover();

    // 冻结/减速：使僵尸速度降低（并可选同步动画间隔）。
    void freeze();

    // 获取僵尸类型。
    zombieName getName() const { return name; }
};