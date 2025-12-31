#include <gtest/gtest.h>

#include "Game/Controllers/BattleController.h"

TEST(GridMapping, WorldToGridAndBackIsConsistent) {
    // BattleController assumes:
    // col = (x - 30) / 80, row = (y - 90) / 100
    for (int col = 0; col < 9; ++col) {
        for (int row = 0; row < 5; ++row) {
            const double x = col * 80 + 30 + 1;   // inside the cell
            const double y = row * 100 + 90 + 1;  // inside the cell
            const Point grid = ABattleController::FindCoordinate(Point(x, y));
            EXPECT_EQ((int)grid.x, col);
            EXPECT_EQ((int)grid.y, row);
        }
    }
}

TEST(GridMapping, OutOfFieldReturnsNegative) {
    const Point grid1 = ABattleController::FindCoordinate(Point(0, 0));
    EXPECT_LT(grid1.x, 0);
    EXPECT_LT(grid1.y, 0);
}
