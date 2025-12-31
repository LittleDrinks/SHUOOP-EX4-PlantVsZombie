# ep4 复现操作指南（下一次：v0.1.0-alpha.3 —— 冰冻豌豆减速僵尸）

你现在不缺“会写类”，你缺的是：**下一版只做一个闭环，不要同时搞 UI/关卡/特效。**

这份文档只做一件事：
- 让 `IcePea` 命中僵尸后，僵尸进入“减速”状态一段时间，然后自动恢复。

---

## 0) 这一版只做 3 件事（别加戏）

### 0.1 版本目标（一句话）
**冰冻豌豆命中僵尸 → 僵尸减速（走路变慢）→ 过一段时间恢复正常。**

### 0.2 Done Definition（做完就算过）
- `IceShooter` 能定时发射 `IcePea`。
- `IcePea` 命中僵尸：
  - 造成伤害（可沿用普通豌豆的 1 点）
  - 触发僵尸减速（例如 3 秒）
  - 自己销毁
- 僵尸减速期间：移动速度变慢；到时间自动恢复。

> 注意：这版不做雪花特效、不做音效、不做 UI 文案。

---

## 1) 核心逻辑对照（别乱命名）

| 功能 | 原项目 (PvZ2) | 你这边 (ep4) |
| :--- | :--- | :--- |
| 冰豌豆子弹 | `IcePea` | `IcePea`（新增） |
| 冰豌豆植物 | `IceShooter` | `IceShooter`（新增） |
| 僵尸减速 | `freeze()/recover()` | `BaseZombie::freeze()/recover()`（落地实现） |
| 减速持续时间 | 常见 3s 左右 | `slowTimer`（新增，3s 一次性触发） |

---

## 2) 第一步：新增 IcePea 子弹（照着 Pea 抄）

**新增文件**：
- `ep4/Game/Entities/Bullets/IcePea.h`
- `ep4/Game/Entities/Bullets/IcePea.cpp`

### 2.1 继承关系（别想太多）
- `IcePea : public BaseBullet`

### 2.2 构造函数要做的事
- 设置速度（建议和豌豆一致 `Point(3.0, 0.0)`）
- 加载贴图：`../res/animations/bullets/icepea.png`
- 设置碰撞箱大小（先和豌豆一样 `25x25`）

### 2.3 update() 固定流程（顺序别乱）
1) 先读碰撞结果：`box->getCollisions("Zombie")`
2) 命中就：
   - `zombie->getAttack(1)`
   - `zombie->freeze()`
   - `Destroy()`
3) 没命中才移动 `addPosition(speed)`
4) 越界销毁（同 Pea）

---

## 3) 第二步：新增 IceShooter（照着 PeaShooter 抄）

**新增文件**：
- `ep4/Game/Entities/Plants/IceShooter.h`
- `ep4/Game/Entities/Plants/IceShooter.cpp`

### 3.1 继承关系
- `IceShooter : public BasePlant`

### 3.2 构造函数要做的事
- 设置 hp/cost（先随便给个合理值：hp=10，cost=175；你以后再对齐原版）
- 加动画：资源 `../res/animations/plants/iceshooter.png`
- 用 `Timer<IceShooter>` 定时调用 `shoot()`

### 3.3 shoot()
- 生成位置参考 `PeaShooter`：`getWorldPosition() + Point(70, 0)`
- 创建 `IcePea`：`GameStatic::createObject<IcePea>(spawnPos)`

---

## 4) 第三步：把 BaseZombie 的 freeze/recover 真正实现出来

你在 `BaseZombie.h` 已经写了接口，但 cpp 里是空的。

**修改文件**：
- `ep4/Game/Entities/Base/BaseZombie.h`
- `ep4/Game/Entities/Base/BaseZombie.cpp`

### 4.1 你要新增一个“恢复计时器”
在 `BaseZombie` 里加：
- `Timer<BaseZombie> slowTimer;`

绑定方式（建议 3.0 秒恢复一次，不循环）：
- `slowTimer.bind(3.0, &BaseZombie::recover, this, false);`

### 4.2 freeze()（减速）最小正确逻辑
- 把 `slowFlag` 设置为 `1`（你现在的走路逻辑里 `slowFlag/2.0` 会让速度减半）
- 重置计时器：`slowTimer.reset()`

> 关键点：**反复吃到冰豌豆时要刷新持续时间**，所以 freeze() 里要 reset。

### 4.3 recover()（恢复）最小正确逻辑
- 把 `slowFlag` 设置回 `2`

### 4.4 update() 里别忘了 tick
无论走路/吃，都要让 “恢复计时器” 走字，否则永远不会恢复：
- 在 `BaseZombie::update()` 里合适位置加 `slowTimer.tick()`（建议每帧都 tick 一次）。

---

## 5) 验收清单（按这个看，别凭感觉）

运行游戏后：

1) 场上有 `IceShooter`，并且能持续发射 `IcePea`（能看到子弹飞行）。
2) `IcePea` 命中僵尸时：
   - 子弹立刻消失
   - 僵尸血量确实减少（死得更快/或能观察到进入死亡状态）
   - 僵尸走路明显变慢
3) 约 3 秒后：僵尸速度恢复正常。
4) 多发冰豌豆连续命中：减速持续时间会被刷新（不是第一次 3 秒到点就恢复）。

如果这四条都满足，v0.1.0-alpha.3 结束，不许继续加功能。
