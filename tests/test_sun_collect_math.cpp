#include <gtest/gtest.h>

#include "Game/Entities/Collectables/Sun.h"

TEST(SunCollect, ReachTargetWithinEpsilon) {
    EXPECT_TRUE(Sun::HasReachedCollectTarget(Point(0.0, 0.0), 0.1));
    EXPECT_TRUE(Sun::HasReachedCollectTarget(Point(3.0, 4.0), 5.0));
    EXPECT_FALSE(Sun::HasReachedCollectTarget(Point(10.0, 0.0), 9.9));
}

TEST(SunCollect, StepMovesTowardTarget) {
    const Point start(100.0, 0.0);
    const Point next = Sun::StepCollectMove(start, 0.1, 2.0);
    // target is (0,0), so x should decrease toward 0
    EXPECT_LT(next.x, start.x);
    EXPECT_DOUBLE_EQ(next.y, start.y);
}

TEST(SunCollect, EventuallyReachesTarget) {
    Point pos(200.0, 150.0);
    for (int i = 0; i < 2000; ++i) {
        pos = Sun::StepCollectMove(pos, 0.01, 2.2);
        if (Sun::HasReachedCollectTarget(pos, 10.0)) {
            SUCCEED();
            return;
        }
    }
    FAIL() << "Sun did not reach collect target within iterations";
}
