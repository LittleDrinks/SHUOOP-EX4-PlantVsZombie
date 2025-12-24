# ep4 复现操作指南（第 03 次：僵尸吃植物）

**前提**：你已经实现了 `Timer` 类，并且 `PeaShooter` 已经能用 Timer 发射子弹了。

**本次目标**：让僵尸碰到植物时停下来，每隔一段时间咬一口，直到植物死亡。

---

## 0. 第零步：检查并补充 Timer 的暂停功能

你提到 Timer 似乎没有暂停功能。确实，目前的 `Timer` 依赖全局的 `Overall.h` 里的 `gameLastTimes` 来处理全局暂停，但缺少**单独暂停某个 Timer** 的功能（比如未来做“冰冻豌豆”效果时可能需要）。

为了保险起见，请先在 `Timer.h` 中补充 `pause()` 和 `resume()` 接口。

**修改文件**：`ep4/Engine/Timer.h`

### 0.1 添加成员变量
在 `private` 区域添加：
```cpp
bool bPaused = false;
DWORD pauseStartTime = 0;
```

### 0.2 添加/修改成员函数
在 `public` 区域添加 `pause` 和 `resume`，并修改 `tick`：

```cpp
// 暂停当前计时器
void pause() {
    if (bPaused) return;
    bPaused = true;
    // 暂停时从全局表中移除，避免被全局 Continue 重复补偿
    gameLastTimes.erase(&lastTime);
    pauseStartTime = timeGetTime();
}

// 恢复当前计时器
void resume() {
    if (!bPaused) return;
    bPaused = false;
    DWORD now = timeGetTime();
    // 补上暂停的这段时间
    lastTime += (now - pauseStartTime);
    // 重新加入全局表
    gameLastTimes.insert(&lastTime);
}

// 修改 tick 函数，增加暂停判断
void tick() {
    // 如果暂停了，就不走字
    if (pOwner == nullptr || bPaused) {
        return;
    }
    // ... 原有逻辑 ...
    if (getDelay() >= delay) {
        (pOwner->*function)();
        if (bPersistant) {
            lastTime = timeGetTime();
        } else {
            pOwner = nullptr;
        }
    }
}
```

### 0.3 修改析构函数
确保析构时不会重复移除（虽然 `std::set::erase` 是安全的，但逻辑上要清晰）：
```cpp
~Timer() {
    // 如果没暂停，说明还在表中，需要移除
    // 如果暂停了，说明已经移除了，不用管
    if (!bPaused) {
        gameLastTimes.erase(&lastTime);
    }
}
```

---

## 1. 核心逻辑对照

| 功能 | 原项目 (PvZ2) | 你这边 (ep4) |
| :--- | :--- | :--- |
| **吃计时器** | `EatTimeHandle` | `eatTimer` (新增) |
| **吃动作** | `AZombie::Eat()` | `BaseZombie::eat()` |
| **植物受击** | `APlant::GetAttack(int)` | `BasePlant::takeDamage(int)` (新增) |

---

## 2. 第一步：给植物加“扣血”功能

僵尸要吃植物，植物得能“掉血”。

**修改文件**：`ep4/Game/Entities/Base/BasePlant.h` 和 `.cpp`

### 2.1 修改头文件 (.h)
在 `public` 区域添加一个函数声明：
```cpp
// 受到伤害
// harm: 伤害值
void takeDamage(int harm);
```

### 2.2 修改实现文件 (.cpp)
实现这个函数：
```cpp
void BasePlant::takeDamage(int harm) {
    hp -= harm;
    // 如果血量归零，就销毁
    if (hp <= 0) {
        Destroy();
    }
}
```

---

## 3. 第二步：给僵尸加“吃”的计时器

僵尸不能每帧都吃（那样植物瞬间就没了），要按节奏吃（比如 0.6秒一口）。

**修改文件**：`ep4/Game/Entities/Base/BaseZombie.h`

### 3.1 添加头文件引用
确保引用了 Timer：
```cpp
#include "../../../Engine/Timer.h"
```

### 3.2 添加成员变量
在 `protected` 或 `private` 里加一个计时器：
```cpp
Timer<BaseZombie> eatTimer;
```

---

## 4. 第三步：实现僵尸的“吃”逻辑

**修改文件**：`ep4/Game/Entities/Base/BaseZombie.cpp`

### 4.1 在构造函数里绑定计时器
僵尸一出生就要把计时器设置好（虽然还没开始跑）：
```cpp
// 在 BaseZombie::BaseZombie() 里添加：
// 绑定 eat 函数，0.6秒触发一次，true 表示循环触发
eatTimer.bind(0.6, &BaseZombie::eat, this, true);
```

### 4.2 实现 eat() 函数
这个函数是计时器触发时调用的：
```cpp
void BaseZombie::eat() {
    // 1. 检查有没有东西可吃
    if (collisions.empty()) return;

    // 2. 取出第一个碰到的植物
    // 注意：collisions 里的对象可能已经销毁了，所以要小心
    // 这里简单取第一个，更严谨的写法需要判断有效性
    BasePlant* plant = (BasePlant*)collisions[0];

    // 3. 咬它！
    if (plant) {
        plant->takeDamage(1); // 假设一口掉1点血
    }
}
```

### 4.3 修改 update()
只有在“吃”的状态下，计时器才走字；“走”的状态下，僵尸移动。

```cpp
void BaseZombie::update() {
    // 1. 先做决策（判断是该走还是该吃）
    judge();

    // 2. 根据状态执行动作
    if (state == 0) {
        // 走路状态：移动
        addPosition(speed * (slowFlag / 2.0));
    } 
    else if (state == 1) {
        // 吃状态：不移动，计时器走字
        eatTimer.tick();
    } 
    else {
        // 死亡状态
        Destroy();
    }
}
```

### 4.4 修改 judge()
这是 AI 的大脑，决定当前是什么状态。

```cpp
void BaseZombie::judge() {
    // 1. 如果血量没了，就死
    if (hp <= 0) {
        state = 2;
        return;
    }

    // 2. 获取当前碰到的植物
    // 注意：这里要清空旧列表，重新获取
    std::vector<Object*> buf = box->getCollisions("Plant");
    collisions.clear();

    // 筛选出有效的 BasePlant
    for (auto& obj : buf) {
        // 简单的类型转换，确保安全可以使用 dynamic_cast 或者自定义的 Cast
        if (BasePlant* p = (BasePlant*)obj) {
            collisions.push_back(p);
        }
    }

    // 3. 状态切换逻辑
    if (!collisions.empty()) {
        // 碰到植物了 -> 切换到吃
        state = 1;
        // 这里可以加：播放吃动画 anim->play("eat");
    } else {
        // 没碰到植物 -> 切换到走
        state = 0;
        // 这里可以加：播放走动画 anim->play("walk");
    }
}
```

---

## 5. 验收清单

做完上面这些后，运行游戏：

1.  **种一个植物**（比如豌豆射手）在僵尸的必经之路上。
2.  **观察僵尸**：
    *   走到植物面前停下了吗？（`state` 切换成功）
    *   植物过一会消失了吗？（`eatTimer` 和 `takeDamage` 工作正常）
    *   植物消失后，僵尸继续往前走了吗？（`judge` 重新切回走路状态）

如果都实现了，恭喜你，核心战斗循环（攻击/受击/死亡/状态切换）已经闭环了！

