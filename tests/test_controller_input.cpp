#include <gtest/gtest.h>

#include "Engine/Objects/Controller.h"

TEST(ControllerInput, ClickUpdatesCoordinatesWithoutMouseMove) {
    Controller c;

    ExMessage down{};
    down.message = WM_LBUTTONDOWN;
    down.x = 123;
    down.y = 45;

    c.ApplyMessage(down);

    const ExMessage& msg = c.GetCursorMessage();
    EXPECT_EQ(msg.x, 123);
    EXPECT_EQ(msg.y, 45);
    EXPECT_TRUE(msg.lbutton);
}

TEST(ControllerInput, ButtonUpClearsLbuttonAndKeepsCoordinates) {
    Controller c;

    ExMessage down{};
    down.message = WM_LBUTTONDOWN;
    down.x = 10;
    down.y = 20;
    c.ApplyMessage(down);

    ExMessage up{};
    up.message = WM_LBUTTONUP;
    up.x = 10;
    up.y = 20;
    c.ApplyMessage(up);

    const ExMessage& msg = c.GetCursorMessage();
    EXPECT_EQ(msg.x, 10);
    EXPECT_EQ(msg.y, 20);
    EXPECT_FALSE(msg.lbutton);
}

TEST(ControllerInput, MouseMoveUpdatesCoordinates) {
    Controller c;

    ExMessage move{};
    move.message = WM_MOUSEMOVE;
    move.x = 777;
    move.y = 888;

    c.ApplyMessage(move);

    const ExMessage& msg = c.GetCursorMessage();
    EXPECT_EQ(msg.x, 777);
    EXPECT_EQ(msg.y, 888);
    // Mouse move should not force lbutton true
    EXPECT_FALSE(msg.lbutton);
}
