#pragma once

#include <graphics.h>

#include "../coreMinimal.h"


class Controller : public Object {
   private:
    // 鼠标位置（窗口坐标，像素）。由 handleInput() 在 WM_MOUSEMOVE 时更新。
    Point mousePosition;

    // 鼠标左键状态。
    // true：收到 WM_LBUTTONDOWN 且未收到 WM_LBUTTONUP。
    // false：初始值为 false；收到 WM_LBUTTONUP 时置 false。
    bool leftMouseClicked = false;

   public:
    Controller() {}
    ~Controller() {}
    void update() override {}

    // 处理输入。
    // 行为：
    // - 循环 peekmessage(&msg, EM_MOUSE | EM_KEY) 清空输入队列。
    // - 更新 mousePosition 与 leftMouseClicked。
    // 调用时机：主循环每帧调用一次（StartGame 中已调用）。
    void handleInput();

    // 获取鼠标位置（窗口坐标，像素）。
    Point getMousePosition() { return mousePosition; }

    // 是否处于按下状态（不是“瞬时点击”事件）。
    bool isLeftMouseClicked() { return leftMouseClicked; }
};