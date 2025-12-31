#pragma once

#include <vector>

#include "../Entities/Collectables/Sun.h"
#include "../Entities/Zombies/NormalZombie.h"
#include "../../Engine/GameStatic.h"
#include "../../Engine/coreMinimal.h"

// BaseLevel：关卡基类，负责按照时间表生成僵尸和掉落阳光。
class BaseLevel : public Object {
protected:
    struct SpawnEvent {
        double time;  // 触发时间（秒）
        int row;      // 0~4 行
        int type;     // 僵尸类型枚举，当前仅 0=Normal
    };

    std::vector<SpawnEvent> zombiePlan;
    size_t nextSpawn = 0;
    double elapsed = 0.0;

    double skySunTimer = 0.0;
    double skySunInterval = 7.0;

    void spawnZombie(int type, int row);
    void spawnSkySun();

    Point gridToWorld(int col, int row) const { return Point(col * 80 + 30, row * 100 + 90); }

public:
    BaseLevel() = default;
    virtual ~BaseLevel() = default;

    virtual void initPlan() = 0;

    void update() override;
};
