#include <gtest/gtest.h>

#include "Game/Entities/Base/BasePlant.h"

TEST(PlantEnabled, DisabledPlantDoesNotTakeDamage) {
    BasePlant p;
    p.setEnabled(false);
    const int before = p.getHp();
    p.takeDamage(999);
    EXPECT_EQ(p.getHp(), before);
}

TEST(PlantEnabled, EnabledPlantTakesDamage) {
    BasePlant p;
    p.setEnabled(true);
    const int before = p.getHp();
    p.takeDamage(1);
    EXPECT_EQ(p.getHp(), before - 1);
}
