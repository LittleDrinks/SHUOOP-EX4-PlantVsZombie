
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
    GameStatic::createObject<NormalZombie>(Point(600, 300));
    GameStatic::createObject<PeaShooter>(Point(200, 350));
}

void StartGame() {
    // 初始化窗口
    Init();

    // 游戏主循环 (Game Loop)
    // 使用 while 循环替代 timeSetEvent，避免多线程渲染问题
    ExMessage msg;
    auto lastTime = std::chrono::high_resolution_clock::now();

    bool running = true;

    while (running) {
        // 计算 DeltaTime
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastTime;
        // 如果帧率过高，可以适当休眠
        if (elapsed.count() < DELTA_TIME) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        lastTime = currentTime;

        // 处理输入
        while (peekmessage(&msg)) {
            // 这里可以分发消息给 Controller
            if (msg.message == WM_QUIT || msg.message == WM_CLOSE) {
                running = false;
                break;
            }
        }
        if (!running) break;
        if (GameStatic::getPlayerController()) {
            GameStatic::getPlayerController()->handleInput();
        }

        // 删除队列处理：先删 Object 再删 Component。
        // 原顺序（先删组件）在“delete obj -> Object::~Object -> root->Destruct()”时
        // 会把组件加入 DGameComponents，但这些组件要等到下一帧才会被真正 delete。
        // 本帧稍后仍会遍历 GameComponents 调用 update()，从而访问已被 delete 的 owner
        // 或父组件指针，常见崩溃点就是 Component::getWorldRotation()/getWorldPosition()。
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

        // 渲染
        cleardevice();
        for (auto& vc : GameRenders) {
            vc->Render();
        }

        FlushBatchDraw();
    }

    EndBatchDraw();
    closegraph();
}