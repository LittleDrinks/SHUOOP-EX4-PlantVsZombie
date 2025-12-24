
#include <set>
#include <thread>

#include "../Game/Entities/Plants/PeaShooter.h"
#include "../Game/Entities/Zombies/NormalZombie.h"
#include "../Game/UI/Background.h"
#include "Components/Camera.h"
#include "Components/Renderer.h"
#include "GameEntry.h"
#include "GameStatic.h"
#include "Objects/Controller.h"
#include "Objects/GameInstance.h"
#include "Overall.h"
#include "Resources.h"
#include "Timer.h"
#include "coreMinimal.h"

void Init() {
    initgraph(WIN_WIDTH, WIN_HEIGHT);
    BeginBatchDraw();
    setbkmode(TRANSPARENT);
    Background* bg = GameStatic::createObject<Background>(Point(0, 0));
    GameStatic::createObject<NormalZombie>(Point(270, 300));
    GameStatic::createObject<PeaShooter>(Point(200, 350));
}

void StartGame() {
    // 初始化窗口
    Init();

    // 游戏主循环 (Game Loop)
    // 使用 while 循环替代 timeSetEvent，避免多线程渲染问题
    ExMessage msg;
    auto lastTime = std::chrono::high_resolution_clock::now();
    double accumulator = 0.0;

    bool running = true;

    while (running) {
        // 真实帧时间（秒）
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> frame = currentTime - lastTime;
        lastTime = currentTime;

        // 防止调试断点/卡顿导致单帧时间过大，出现“追帧更新太久”的假死
        double frameSeconds = frame.count();
        if (frameSeconds > 0.25) {
            frameSeconds = 0.25;
        }
        accumulator += frameSeconds;

        // 处理输入（按真实帧频率处理即可）
        while (peekmessage(&msg)) {
            if (msg.message == WM_QUIT || msg.message == WM_CLOSE) {
                running = false;
                break;
            }
        }
        if (!running) break;
        if (GameStatic::getPlayerController()) {
            GameStatic::getPlayerController()->handleInput();
        }

        // 固定时间步长更新：无论渲染帧率如何，逻辑以 DELTA_TIME 稳定推进
        while (accumulator >= DELTA_TIME) {
            // 删除队列处理：先删 Object 再删 Component。
            for (auto& obj : DGameObjects) {
                GameObjects.erase(obj);
                delete obj;
            }
            DGameObjects.clear();

            for (auto& comp : DGameComponents) {
                GameComponents.erase(comp);
                delete comp;
            }
            DGameComponents.clear();

            // 逻辑更新
            for (auto& comp : GameComponents) {
                comp->update();
            }
            for (auto& obj : GameObjects) {
                obj->update();
            }

            // 碰撞判定
            CollisionJudge();

            accumulator -= DELTA_TIME;
        }

        // 渲染（按显示帧率渲染即可）
        cleardevice();
        for (auto& vc : GameRenders) {
            vc->Render();
        }
        FlushBatchDraw();

        // 轻微让出 CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EndBatchDraw();
    closegraph();
}