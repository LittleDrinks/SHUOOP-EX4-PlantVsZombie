#include "Controller.h"

void Controller::handleInput() {
    ExMessage msg;

    // 循环处理所有积压的信息
    while (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
        if (msg.message == WM_MOUSEMOVE) {
            mousePosition = Point(msg.x, msg.y);
        } else if (msg.message == WM_LBUTTONDOWN) {
            leftMouseClicked = true;
        } else if (msg.message == WM_LBUTTONUP) {
            leftMouseClicked = false;
        }
    }
}