#pragma once

#include "../Base/BaseBullet.h"
#include "../../../Engine/Components/Animator.h"
#include "../../../Engine/GameStatics.h"
#include "../../../Engine/Timer.h"

#include <cmath>

// Sun：可收集的阳光。
// - 默认自空中坠落到 landingY 停住，若玩家点击则飞向左上角收集栏。
// - 定时淡出并销毁；收集后为玩家增加 25 阳光。
class Sun : public BaseBullet {
private:
    Animator* anim = nullptr;
    double landingY = 0.0;
    bool falling = true;
    bool collected = false;
    bool fading = false;

    double fallSpeed = 1.2;      // 下落速度（像素/帧）
    double collectSpeed = 8.0;   // 收集时的匀速系数（越大飞向左上角越快）

    Timer<Sun> fadeTimer;     // 到时间后进入淡出
    Timer<Sun> destroyTimer;  // 最终销毁

    void beginFade();
    void removeSelf();

public:
    Sun();

    void setLandingY(double y) { landingY = y; }

    void update() override;

    // 简易点击测试：判断 point 是否落在碰撞箱矩形内。
    bool hitTest(const Point& p);

    // 便于测试的纯逻辑：收集目标点与到达判定。
    static Point CollectTarget() { return Point(0.0, 0.0); }
    static bool HasReachedCollectTarget(const Point& pos, double eps = 10.0) {
        const Point t = CollectTarget();
        const double dx = pos.x - t.x;
        const double dy = pos.y - t.y;
        return std::sqrt(dx * dx + dy * dy) <= eps;
    }
    static Point StepCollectMove(const Point& pos, double dt, double speed) {
        const Point t = CollectTarget();
        return pos + (t - pos) * (speed * dt);
    }
};
