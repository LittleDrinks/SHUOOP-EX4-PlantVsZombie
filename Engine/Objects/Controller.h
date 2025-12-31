#pragma once

#include <graphics.h>

#include "../coreMinimal.h"

#include "../Pvz2CompatGlobals.h"


class Controller : public Object {
   private:
    // 鼠标位置（窗口坐标，像素）。由 handleInput() 在 WM_MOUSEMOVE 时更新。
    Point mousePosition;

    // 鼠标左键状态。
    // true：收到 WM_LBUTTONDOWN 且未收到 WM_LBUTTONUP。
    // false：初始值为 false；收到 WM_LBUTTONUP 时置 false。
    bool leftMouseClicked = false;

    // PVZ2-like：最近一次输入消息（用于 GetCursorMessage 接口）。
    ExMessage msg{};

    void syncMsg();

   public:
    Controller() {}
    ~Controller() {}
    void update() override {}

    // 处理单条输入消息（便于复用与单元测试）。
    void ApplyMessage(const ExMessage& m);

    // 处理输入。
    // 行为：
    // - 循环 peekmessage(&msg, EM_MOUSE | EM_KEY) 清空输入队列。
    // - 更新 mousePosition 与 leftMouseClicked。
    // 调用时机：主循环每帧调用一次（StartGame 中已调用）。
    void handleInput();

    // PVZ2-like：与原项目同名接口，语义等价于“读取并更新输入状态”。
    void PeekInfo() { handleInput(); }

    // PVZ2-like：返回最近一次消息（其中 lbutton 会被同步为当前按下状态）。
    const ExMessage& GetCursorMessage() const { return msg; }

    // PVZ2-like：返回鼠标屏幕坐标（像素）。
    Point GetCursorScreenPosition() const { return Point(mousePosition.x, mousePosition.y); }

    // PVZ2-like：当前工程未实现相机变换，世界坐标与屏幕坐标一致。
    Point GetCursorWorldPosition() const { return GetCursorScreenPosition(); }

    // PVZ2-like：将该 Controller 设为全局主控制器。
    void SetPossession() { MainController = this; }

    // 获取鼠标位置（窗口坐标，像素）。
    Point getMousePosition() { return mousePosition; }

    // 是否处于按下状态（不是“瞬时点击”事件）。
    bool isLeftMouseClicked() { return leftMouseClicked; }
};