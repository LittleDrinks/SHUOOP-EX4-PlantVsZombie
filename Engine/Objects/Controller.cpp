#include "Controller.h"

void Controller::syncMsg() {
    msg.x = (int)mousePosition.x;
    msg.y = (int)mousePosition.y;
    msg.lbutton = leftMouseClicked;
}

void Controller::ApplyMessage(const ExMessage& m) {
    msg = m;

    if (m.message == WM_QUIT || m.message == WM_CLOSE) {
        QuitRequested = true;
    }

    // 关键修复：很多情况下“点击”不会伴随 WM_MOUSEMOVE。
    // 如果不在按下/抬起事件里同步坐标，那么 msg.x/msg.y 会停留在上一次移动位置，
    // 导致点击命中检测（例如阳光收集）失败。
    if (m.message == WM_MOUSEMOVE || m.message == WM_LBUTTONDOWN || m.message == WM_LBUTTONUP) {
        mousePosition = Point(m.x, m.y);
    }

    if (m.message == WM_LBUTTONDOWN) {
        leftMouseClicked = true;
    } else if (m.message == WM_LBUTTONUP) {
        leftMouseClicked = false;
    }

    syncMsg();
}

void Controller::handleInput() {
    ExMessage m;

    // 循环处理所有积压的信息
    while (peekmessage(&m)) {
        ApplyMessage(m);
    }

    // 即使本帧没有新消息，也保证 msg 与当前状态一致。
    syncMsg();
}