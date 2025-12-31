#include <gtest/gtest.h>

#include "Engine/Objects/UserInterface.h"

TEST(TextColor, SetAndGet) {
    Text t;
    t.SetColor(RGB(0,0,0));
    EXPECT_EQ(t.GetColor(), RGB(0,0,0));
}
