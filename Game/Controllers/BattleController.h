#pragma once

#include "../../Engine/Objects/Controller.h"
#include "../../Engine/GameStatics.h"
#include "../Entities/Base/BasePlant.h"
#include "../Entities/Plants/SunFlower.h"

// PVZ2-like global grid
extern BasePlant* plants[9][5];

class UBattleUI;

// PVZ2-like: minimal ABattleController (only plant placement flow)
class ABattleController : public Controller {
    static constexpr int kCols = 9;
    static constexpr int kRows = 5;
    static constexpr int kCardCount = 2;

    UBattleUI* ui = nullptr;
    BasePlant* plant = nullptr;
    BasePlant* prePlant = nullptr;
    int curIndex = -1;
    bool prevLbutton = false;

    int cardCosts[kCardCount] = {100, 50};
    double cardCooldown[kCardCount] = {7.0, 7.0};
    double cardRemaining[kCardCount] = {0.0, 0.0};

    BasePlant* spawnPlantByIndex(int index, Point pos);
    bool canUseCard(int index) const;
    void startCooldown(int index);
    void tickCooldowns();
    void refreshCardVisuals();
    void cleanDeadPlants();

public:
    ABattleController();
    void update() override;

    static Point FindCoordinate(Point pos) {
        int x = int(pos.x - 30);
        int y = int(pos.y - 90);
        if (x < 0 || y < 0) return Point(-1, -1);
        return Point(x / 80, y / 100);
    }
};
