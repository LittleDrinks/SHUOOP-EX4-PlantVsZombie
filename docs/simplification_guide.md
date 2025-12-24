# ep4 复现操作指南（下一个版本：v0.1.0-alpha.2 —— 僵尸吃植物闭环）

你现在不缺“想法”，你缺的是：**下一版到底只做哪几件事、做到什么程度算完成。**

这份指南只干两件事：
1) 明确 v0.1.0-alpha.2 的目标（别加戏）
2) 给你一套“照着改文件就能过验收”的步骤

---

## 0) 这一版只做 4 件事（写进 commit message 的那种）

### 0.1 版本目标（一句话）
**僵尸能靠碰撞箱识别植物 → 停下 → 按节奏咬植物 → 植物死 → 僵尸继续走。**

### 0.2 Done Definition（做完就算过）
- 工程能编译运行（不允许“缺个函数名但我懂就行”）。
- `CollisionJudge()` 真正存在，并且每帧会把碰撞关系写进 `BoxCollider::boxes`。
- `BasePlant` 有明确的“扣血并死亡销毁”接口。
- `BaseZombie` 吃植物不是每帧吃，而是用 `Timer` 控制频率（比如 0.6 秒一口）。

---

## 1) 第一步：先把碰撞系统接上（不然后面全是假的）

你现在的主循环已经在调用 `CollisionJudge();`，但工程里还缺这个函数的真实实现。

**你要记住一句话**：
> **只有引擎每帧先算完碰撞，`box->getCollisions("Plant")` 才可能拿到东西。**

### 1.1 你要做什么
实现一个全局函数：`void CollisionJudge();`

### 1.2 推荐放哪
最简单做法：
- 在 `ep4/Engine/Components/BoxCollider.h` 里声明 `void CollisionJudge();`
- 在 `ep4/Engine/Components/BoxCollider.cpp` 里实现它

### 1.3 它必须做什么（最低要求）
每帧：
1) 遍历 `GameColliders_`，对每个 collider 调用 `clear()`
2) 两两检测：`if (a != b) a->Insert(b);`

> 这版先别做四叉树/网格分区优化。能对、能跑、能验收最重要。

---

## 2) 第二步：补 Timer 的“单独暂停/恢复”（给未来留口子）

你当前的 `Timer` 已经能 tick，但缺少“只暂停某一个 Timer”的能力。

**修改文件**：`ep4/Engine/Timer.h`

### 2.1 添加成员变量（private）
```cpp
bool bPaused = false;
DWORD pauseStartTime = 0;
```

### 2.2 添加接口（public）
```cpp
void pause();
void resume();
```

### 2.3 修改 tick
规则就一句：**暂停时不走字、不触发回调。**

---

## 3) 第三步：植物必须能“掉血 + 死亡销毁”

你现在的 `BasePlant` 只有 `hp<=0 就 Destroy()`，但没有明确的“受击接口”。
僵尸要咬你，就必须有这个接口。

**修改文件**：
- `ep4/Game/Entities/Base/BasePlant.h`
- `ep4/Game/Entities/Base/BasePlant.cpp`

### 3.1 在头文件声明
```cpp
void takeDamage(int harm);
```

### 3.2 在 cpp 实现
```cpp
void BasePlant::takeDamage(int harm) {
    hp -= harm;
    if (hp <= 0) {
        Destroy();
    }
}
```

> 注意：你可以继续保留 `update()` 里 `hp<=0` 的兜底销毁，但咬人逻辑必须走 `takeDamage()`。

---

## 4) 第四步：僵尸“走/吃”状态机 + 吃的节奏

你现在的 `BaseZombie` 已经在 `judge()` 里用 `box->getCollisions("Plant")` 取植物了，
但还差两块：
1) `eat()` 真正扣血
2) 吃不是每帧调用，用 `Timer` 限速

**修改文件**：
- `ep4/Game/Entities/Base/BaseZombie.h`
- `ep4/Game/Entities/Base/BaseZombie.cpp`

### 4.1 给 BaseZombie 加一个吃的计时器
在 `.h` 里 include：
```cpp
#include "../../../Engine/Timer.h"
```

加成员：
```cpp
Timer<BaseZombie> eatTimer;
```

### 4.2 在构造里 bind（建议 0.6 秒一口）
```cpp
eatTimer.bind(0.6, &BaseZombie::eat, this, true);
```

### 4.3 实现 eat()（最小正确版本）
```cpp
void BaseZombie::eat() {
    if (collisions.empty()) return;
    BasePlant* plant = collisions[0];
    if (plant) {
        plant->takeDamage(1);
    }
}
```

### 4.4 修改 update()：走路就移动，吃就 tick
```cpp
if (state == 0) {
    addPosition(speed * (slowFlag / 2.0));
} else if (state == 1) {
    eatTimer.tick();
} else {
    Destroy();
}
```

### 4.5 修改 judge()：切状态时做一件关键事
当你从“走路 -> 吃”切换时，建议：
- 立刻 `eatTimer.reset();`（避免刚切换就“立刻触发一口”，节奏更稳定）

---

## 5) 验收清单（你跑一遍就知道过没过）

运行游戏后：

1) 放一个植物在僵尸路线上（PeaShooter 就行）。
2) 观察僵尸：
   - 碰到植物后停下（state 从 0 -> 1）
   - 每隔 ~0.6 秒咬一次（植物 hp 逐步减少）
   - 植物死亡消失（Destroy 生效）
   - 植物消失后僵尸继续走（state 从 1 -> 0）

如果这四条都满足，v0.1.0-alpha.2 结束，不许继续加功能。

