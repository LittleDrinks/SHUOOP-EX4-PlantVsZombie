# ep4 项目结构与派生关系（白痴版，照着写就不会乱）

你现在的问题不是“不会 C++ 语法”，而是“不知道应该把代码写到哪里、每个类该负责什么”。
这份文档只做一件事：把 ep4 的**项目结构**和**派生关系**讲清楚，让你写 Zombie/Bullet 时不会乱。

---

## 1) 30 秒看懂：这个项目分两层

把 `ep4` 当成两层：

1) **Engine（引擎层）**：负责“跑起来”
- 开窗口、每帧循环、收集输入、更新对象、做碰撞、做渲染。
- 你写游戏逻辑时，不应该在 Engine 里到处改。

2) **Game（游戏层）**：负责“规则”
- 僵尸怎么走、子弹怎么飞、植物怎么射击。
- 你写 Zombie/Bullet 的痛苦，应该在 Game 层解决，而不是乱改 Engine。

---

## 2) 你要记住的“游戏对象派生链”（Object 这一条线）

一句话：**你写的僵尸/植物/子弹，都是 Object 的派生。**

这条链你只需要背下来：

- `Object`（引擎基类：位置/旋转/Destroy/每帧 update）
  - `StaticMesh`（引擎提供的“带默认渲染器的 Object”）
    - `BasePlant`（植物公共基类）
      - `PeaShooter`（豌豆射手）
    - `BaseZombie`（僵尸公共基类）
      - `NormalZombie`（普通僵尸）
    - `BaseBullet`（子弹公共基类）
      - `Pea`（豌豆子弹）

你可以在这些文件里看到继承关系：
- `StaticMesh` 定义：`ep4/Engine/Components/Renderer.h`
- 植物/僵尸/子弹基类：
  - `ep4/Game/Entities/Base/BasePlant.h`
  - `ep4/Game/Entities/Base/BaseZombie.h`
  - `ep4/Game/Entities/Base/BaseBullet.h`

你只需要记住一句话：
- **只要你想让一个东西能显示出来，就让它继承 `StaticMesh`。**

---

## 3) 你要记住的“组件派生链”（Component 这一条线）

一句话：**渲染/动画/碰撞都是 Component（组件）。**

你不用记全部细节，记下面三类就够用了：

### A) 渲染组件
- `Component`
  - `VisualComponent`（可渲染组件：会进 `GameRenders`，每帧会被 Render）
    - `SpriteRenderer`（真正画图片/裁剪动画帧的）

对应文件：`ep4/Engine/Components/Renderer.h`

### B) 动画组件
- `Component`
  - `Animator`

对应文件：`ep4/Engine/Components/Animator.h`

### C) 碰撞组件
- `Component`
  - `Collider`
    - `BoxCollider`

对应文件：`ep4/Engine/Components/BoxCollider.h`

---

## 4) 谁拥有谁？（你只会语法但写不下去，通常就卡在这里）

### 4.1 每个 Object 内部都有一个 root（组件树根）

- `Object` 构造时会创建 `root = new Component()`。
- 你创建的所有组件（渲染器/碰撞体/动画器）都要 `setAttachment(root)` 挂到这个 root 上。

你可以在这里看到：`ep4/Engine/coreMinimal.cpp` 里的 `Object::Object()`。

### 4.2 StaticMesh 为什么“天生能渲染”？

因为 `StaticMesh` 构造函数里自动做了两件事：
1) 创建 `SpriteRenderer` 组件
2) attach 到 `root`

所以你在 Game 层写类时，经常能直接用 `renderer->load(...)`。
这不是魔法，是 `StaticMesh` 帮你造好了 renderer。

对应位置：`ep4/Engine/Components/Renderer.cpp` 里的 `StaticMesh::StaticMesh()`。

---

## 5) 用一个对象走一遍（你就知道代码该写哪）

### 5.1 创建一个 NormalZombie 会发生什么？

在 `ep4/Engine/game.cpp` 的 `Init()` 里，会调用：
- `GameStatic::createObject<NormalZombie>(...)`

构造链会按顺序跑：
1) `Object()`：创建 root，注册进 `GameObjects_`
2) `StaticMesh()`：创建 `SpriteRenderer`，注册进 `GameRenders`
3) `BaseZombie()`：创建 `Animator` + `BoxCollider`，并把 collider type 设为 `"Zombie"`
4) `NormalZombie()`：配置动画 `anim->addAnimation(...)` 并 `play("walk")`

你写新僵尸时“该写哪里”，答案是：
- 资源/动画：写在 **派生僵尸构造函数** 里
- AI 决策：写在 **派生僵尸 judge()** 里

### 5.2 创建一个 Pea 子弹会发生什么？

构造链会按顺序跑：
1) `StaticMesh()` 先给你造 `renderer`
2) `BaseBullet()` 给你造 `BoxCollider`，并把 type 设为 `"Bullet"`
3) `Pea()` 设置速度、贴图、碰撞体大小

然后每帧：
- `Pea::update()` 用固定流程：查命中 → 命中销毁 → 移动 → 越界销毁

---

## 6) 你最常问的 4 个问题（直接给答案）

### Q1：我写的类到底该继承谁？
A：
- 需要显示在屏幕上 → 继承 `StaticMesh`（Zombie/Plant/Bullet 都是）
- 只是一个零件（动画/碰撞/渲染）→ 继承 `Component`

### Q2：我应该把“画图代码”写在哪？
A：你基本不用写画图代码。
- 画图在 `SpriteRenderer::Render()` 里统一做。
- 你在 Game 层只做：`renderer->load(...)` 或让 `Animator` 写 `aniSource`。

### Q3：动画到底是谁在“驱动”？
A：每帧顺序是：
- `Animator::update()` 写 `Object::aniSource`
- `SpriteRenderer::update()` 读 `aniSource`，Render 时按帧裁剪画出来

### Q4：碰撞到底是谁在算？我在对象里要做什么？
A：
- Engine 每帧统一算：`CollisionJudge()`
- BoxCollider 记录结果：你在对象里只要查询 `GetCollisions("Plant/Zombie")`

---

## 7) 两个问题（你可以在笔记里写答案，写完你就不会乱）

1) 你能接受：只要一个东西要显示，就统一继承 `StaticMesh` 吗？（是/否）
2) 你更想先练：写一个新僵尸（HatZombie）还是写一个新子弹（SnowPea）？（Zombie/Bullet）
