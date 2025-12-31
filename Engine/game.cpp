
#include <set>
#include <thread>
#include <direct.h>
#include <io.h>

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
#include "Objects/UserInterface.h"

#include "Pvz2CompatGlobals.h"

#include "../Game/Controllers/BattleController.h"
#include "../Game/Levels/LevelOne.h"

void Init() {
    // 尽量对齐原项目：资源路径使用 res/...。
    // 若当前工作目录不含 res/，则尝试切到上一级（常见于从 build/ 运行）。
    if (_access("res\\images\\bar.png", 0) != 0) {
        _chdir("..");
    }

    initgraph(WIN_WIDTH, WIN_HEIGHT);
    BeginBatchDraw();
    setbkmode(TRANSPARENT);
    // 对齐原项目资源坐标系：bg5 背景图存在左侧留白区域，原项目通过偏移/相机处理。
    // 当前工程未实现 Camera 变换，这里直接把背景左移，减少草坪格子视觉偏移。
    GameStatic::createObject<Background>(Point(-225, 0));

    // PVZ2-like controller & UI
    GameStatic::createObject<ABattleController>(Point(0, 0));

    // Level flow
    GameStatic::createObject<LevelOne>(Point(0, 0));
}

void StartGame() {
    // 初始化窗口
    Init();

    // 游戏主循环 (Game Loop)
    // 使用 while 循环替代 timeSetEvent，避免多线程渲染问题
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

        // 处理输入：由 Controller 统一消费消息队列，避免消息被重复读取/提前丢失
        if (MainController) {
            MainController->handleInput();
        } else if (GameStatic::getPlayerController()) {
            GameStatic::getPlayerController()->handleInput();
        }
        if (QuitRequested) {
            running = false;
            break;
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

            // UI 更新（PVZ2-like）
            for (auto* ui : GameUIs) {
                ui->Update();
            }

            // UI 删除队列
            if (!GameUIs_.empty()) {
                for (auto* ui : GameUIs_) {
                    GameUIs.erase(ui);
                    delete ui;
                }
                GameUIs_.clear();
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

        // UI Painter 渲染（PVZ2-like）
        for (auto* p : GamePainters) {
            p->Paint();
        }
        FlushBatchDraw();

        // 轻微让出 CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    EndBatchDraw();
    closegraph();
}