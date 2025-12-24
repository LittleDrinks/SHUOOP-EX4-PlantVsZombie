#pragma once

#include "../Base/BaseZombie.h"

// NormalZombie：普通僵尸。
// 职责：
// - 继承 BaseZombie 的移动/状态机框架。
// - judge() 负责根据碰撞与状态切换（走路/吃植物/死亡）。
class NormalZombie : public BaseZombie {
public:
    // 构造：加载自身动画/渲染资源，初始化碰撞体与参数。
    NormalZombie();

    // 状态判定入口：每帧由 BaseZombie::update()/外部逻辑调用。
    void judge() override;
};