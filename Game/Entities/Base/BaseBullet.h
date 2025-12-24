#pragma once

#include "../../../Engine/Components/BoxCollider.h"
#include "../../../Engine/Components/Renderer.h"
#include "../../../Engine/GameStatic.h"
#include "../../../Engine/coreMinimal.h"

// BaseBullet：子弹对象基类。
// 职责：
// - 提供碰撞体 box。
// - 提供速度 speed，并在派生类 update() 中用于移动。
//
// 约束：
// - speed 单位为像素/帧（当前工程的 update() 每帧调用一次）。
// - box 的创建与挂载必须在实现文件中完成，否则碰撞系统无法检测。
class BaseBullet : public StaticMesh {
protected:
    // 碰撞体（AABB）。用于命中检测。
    // 所有权：通常由对象/组件系统管理；BaseBullet 仅保存指针引用。
    BoxCollider* box;

    // 子弹速度（像素/帧）。
    // 说明：该字段只在派生类 update() 中生效；基类不自动移动。
    Point speed = Point(0.0, 0.0);

public:
    // 构造：负责初始化渲染器/碰撞体的默认状态。
    // 约束：构造结束后如果需要参与碰撞，必须确保 box 已创建并注册到碰撞系统。
    BaseBullet();

    // 每帧更新：派生类覆盖实现移动与碰撞响应。
    // 调用时机：主循环每帧调用一次。
    virtual void update() {}

    // 获取子弹碰撞体指针。
    BoxCollider* getBoxCollider() const { return box; }

    // 设置速度。
    // 约束：只修改速度本身，不负责立刻推进位置；位置推进在 update() 中完成。
    void setSpeed(const Point& spd) { speed = spd; }
};