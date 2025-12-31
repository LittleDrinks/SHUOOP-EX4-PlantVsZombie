#pragma once

#include "BaseLevel.h"

// LevelOne：最小化的白天关卡，定期刷普通僵尸并掉落阳光。
class LevelOne : public BaseLevel {
public:
    LevelOne();
    void initPlan() override;
};
