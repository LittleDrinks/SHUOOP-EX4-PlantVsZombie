#include "LevelOne.h"

LevelOne::LevelOne() {
    skySunInterval = 6.0;
    initPlan();
}

void LevelOne::initPlan() {
    zombiePlan.clear();
    nextSpawn = 0;
    elapsed = 0.0;

    // 基础波次：逐渐加快，保持易于测试且可拓展
    double t = 4.0;
    for (int i = 0; i < 6; ++i) {
        const int row = RandInt(0, 4);
        zombiePlan.push_back({t, row, 0});
        t += 4.5 - i * 0.4;
    }
}
