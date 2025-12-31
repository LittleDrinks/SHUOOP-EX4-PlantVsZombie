#pragma once

#include "../../../Engine/Timer.h"
#include "../Base/BasePlant.h"
#include "../Collectables/Sun.h"

// SunFlower：产阳光植物。
// - 周期性播放生产动画并生成 Sun。
// - 继承 BasePlant，保留碰撞与渲染能力。
class SunFlower : public BasePlant {
private:
    double produceInterval = 8.0;  // 生产周期（秒）
    Timer<SunFlower> produceTimer;
    Timer<SunFlower> revertTimer;

    void startProduce();
    void finishProduce();

public:
    SunFlower();

    void update() override;
};
