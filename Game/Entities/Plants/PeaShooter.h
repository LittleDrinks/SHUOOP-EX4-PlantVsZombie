#pragma once
#include "../../../Engine/Timer.h"
#include "../Base/BasePlant.h"
#include "../Bullets/Pea.h"

// PeaShooter：豌豆射手。
// 职责：
// - 在 update() 中按固定间隔调用 shoot()。
// - shoot() 负责创建 Pea 子弹并设置其初始位置/速度/渲染资源。
class PeaShooter : public BasePlant {
private:
    // 射击间隔（秒）。当 shootTimer >= shootInterval 时触发一次 shoot()。
    double shootInterval = 1.8;

    // 射击定时器对象
    Timer<PeaShooter> tm;

public:
    // 构造：加载动画/渲染资源，初始化碰撞体与参数。
    PeaShooter();

    // 每帧更新：推进射击计时器。
    void update() override;

    // 发射一颗豌豆子弹。
    // 副作用：创建并注册一个 Pea 到全局对象集合（由
    // GameStatic/对象系统管理生命周期）。
    // 约束：必须为子弹设置初始位置/速度/渲染资源，否则会出现“创建了但看不见/不移动”的现象。
    void shoot();
};