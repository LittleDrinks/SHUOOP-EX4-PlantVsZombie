# ep4 复现指南（“内部框架/接口”与原项目 PlantVsZombie2 完全一致）

你说的“完全一致”是指：

- 类名一致：`GameStatics / Controller / UserInterface / Image / Text / UBattleUI / ABattleController ...`
- 成员函数签名一致：例如 `UserInterface::AddWidget<T>()`、`Image::IsCursorOn()`、`Controller::GetCursorMessage()`、`ABattleController::FindCoordinate()`
- 调用链一致：UI 在 `UBattleUI::Update()` 里产出 `index`，Controller 在 `ABattleController::Update()` 里创建/拖拽/落子

这类“内部框架接口 1:1”**不能靠在 ep4 现有引擎里“模仿实现”达成**（因为 ep4 的对象模型/组件模型/输入系统命名都不同）。

唯一稳妥路线：

> 把原项目 `PlantVsZombie2` 的引擎/UI/Controller 框架整套迁入 ep4，并用它来跑游戏。

补充约束：
- 不要发明新的全局/接口名字（例如 `Pvz2CompatGlobals` 这类 ep4 自创命名）；只使用原项目里已有的类/函数/变量名。
- 不要在 ep4 原有 Engine 框架里“桥接”出一套新接口，再映射到原项目；直接用原项目源码里的接口即可。

这样你参考时看到的文件、类、接口、调用位置都与原项目一模一样，你只是在 ep4 目录下维护它。

---

## 0) 你最终要达到的验收标准

1) ep4 能编译出一个“原项目框架版”的可执行程序（下面叫 `pvz2_main`）
2) 运行后，能进入原项目的 UI/控制器链路（至少能看到菜单/进入战斗，取决于你选的入口 Controller）
3) 你对“选卡 + 种植”的修改发生在：
   - `BattleUI.cpp/.h`（UI 产出 `index`）
   - `BattleController.cpp/.h`（Controller 消费 `index` 并落子）
   - 网格映射函数 `FindCoordinate()` 保持原样

---

## 1) 目录准备（把原项目框架搬进 ep4）

### Step 1.1 复制源码到 ep4 内部

操作：
1) 在 `ep4/` 下新建目录：
   - `ep4/pvz2/`
2) 把下面整个目录从原项目复制到 `ep4/pvz2/`：
   - `PlantVsZombie/PlantVsZombie2/*.h`
   - `PlantVsZombie/PlantVsZombie2/*.cpp`
   - `PlantVsZombie/PlantVsZombie2/engine/`（编辑器开关、背景、file.txt 等）
   - `PlantVsZombie/PlantVsZombie2/res/`（图片/音效等资源）

建议（减少干扰）：
- 不要复制 `x64/`、`.sln`、`.vcxproj`、`PlantVsZombie2.exe` 这种生成物。

验收：
- `ep4/pvz2/` 里应该能看到原项目同名文件，比如 `GameStatics.h`、`UserInterface.h`、`BattleUI.cpp`、`BattleController.cpp`、`game.cpp`。

### Step 1.2 资源路径必须保持原样

原项目里大量写死了相对路径（例如 `res/images/...`、`engine/background.png`）。

因此你必须保证：
- 运行时工作目录（Working Directory）指向 `ep4/pvz2/`
  - 否则会出现：图片加载失败、声音加载失败、编辑器文件读取失败。

---

## 2) CMake：在 ep4 工程里新增一个“原项目框架版”可执行目标

你当前 `ep4/CMakeLists.txt` 会把 ep4 自己的 `src/Engine/Game` 全部编进 `ep4_main`。

我们要做的是：

- 保留 `ep4_main`（可选）
- 新增一个 `pvz2_main`，只编译 `ep4/pvz2/` 下的原项目源码

### Step 2.1 新增 pvz2_main

操作：打开 `ep4/CMakeLists.txt`，在现有内容下面追加：

```cmake
# --- 原项目框架版（PlantVsZombie2） ---
file(GLOB_RECURSE PVZ2_SRC
    pvz2/**.cpp
    pvz2/**.h
)

add_executable(pvz2_main ${PVZ2_SRC})
target_link_libraries(pvz2_main libeasyx.a winmm Msimg32)
```

说明：
- 原项目在 `pvz2/game.cpp` 里使用了 `void main()` 作为入口；大多数 Windows 编译链可通过。
- 如果你遇到“找不到 main/WinMain”链接错误：
  - 先不要大改框架；最小改动是把 `pvz2/game.cpp` 末尾的 `void main()` 改成 `int main()`，函数体不变，最后 `return 0;`。

### Step 2.2 避免把 ep4/src/main.cpp 编进 pvz2_main

上面的写法不会包含 ep4 的 `src/`，所以不会冲突。

如果你后续把 `PVZ2_SRC` 写得过宽导致同时编进了 `ep4/src/main.cpp`，会出现“重复 main”链接错误。
解决方法：确保 `pvz2_main` 只吃 `pvz2/**`。

---

## 3) 运行：必须把工作目录设为 ep4/pvz2

### 方式 A（推荐）：VS Code + CMake Tools

操作：
1) 在 VS Code 的 CMake Tools 里把 Launch/Debug Target 选为 `pvz2_main`
2) 找到运行配置（launch.json 或 CMake Tools 的运行设置），把工作目录设为 `ep4/pvz2`

验收：
- 运行后不应出现大面积资源加载失败。

### 方式 B：命令行运行

操作：
1) 先切到 `ep4/pvz2` 再运行 exe（关键是当前目录）
2) 从 `build/bin`（或你的构建输出目录）启动 `pvz2_main.exe`

验收同上。

---

## 4) 你要改“选卡 + 种植”，请只改原项目同名文件（接口保持 1:1）

当你用 `pvz2_main` 跑起来后，接下来“对齐原项目”就很简单了：

### 4.1 UI（完全一致）

文件：
- `ep4/pvz2/BattleUI.h`
- `ep4/pvz2/BattleUI.cpp`

你参考的点：
- `UBattleUI::Update()` 里写 `index / failFlag` 的逻辑
- `Seeds[i]->IsCursorOn()`、`MainController->GetCursorMessage().lbutton` 这套 UI/输入接口

要求：
- 不要改接口名/签名
- 只改业务规则（比如你想先关掉价格/冷却，也是在这里改条件）

### 4.2 Controller（完全一致）

文件：
- `ep4/pvz2/BattleController.h`
- `ep4/pvz2/BattleController.cpp`

你参考的点：
- `ABattleController::Update()` 的 `/* 植物 */` 段
- `FindCoordinate()` 必须保持原项目公式不变（除非你要改草坪对齐）

要求：
- 不要改 `plants[9][5]` 的语义（它就是“网格占用 + 实例存储”）
- 不要把逻辑迁到别的系统里（否则你又回到了“框架不一致”）

### 4.3 禁止新增自定义命名（保持 1:1）

- 不要新增任何“兼容层”或新命名的接口/全局（如 `Pvz2CompatGlobals`、额外的 `MainController` 指针等）。
- 需要的全局/接口都来自原项目源码本身（`GameStatics`、`Controller`、`UserInterface` 等），直接沿用即可。

---

## 5) 常见坑（内部框架 1:1 迁移时最容易卡住的）

### 5.1 资源加载失败（图片/音效一片空白）

原因：工作目录不对。

解决：确保运行时当前目录是 `ep4/pvz2/`，且 `res/`、`engine/` 目录就在它下面。

### 5.2 编译/链接报错：找不到 main/WinMain

原因：原项目入口写的是 `void main()`。

解决（最小改动）：
- 把 `pvz2/game.cpp` 末尾 `void main()` 改成 `int main()`，函数体不动，最后加 `return 0;`。

### 5.3 编译报错：找不到 easyx / AlphaBlend / winmm

解决：
- CMake 目标必须链接：`libeasyx.a winmm Msimg32`

---

## 6) 你现在这一版（ep4 原生引擎）该怎么处理？

如果你的目标是“内部框架完全一致”：

- `ep4/Engine/*`、`ep4/Game/*` 这套可以先不删，但不要再在它上面继续堆 UI/种植逻辑
- 之后你的开发重心转移到 `ep4/pvz2/*`（也就是原项目同名文件）

当你把 `pvz2_main` 跑通并且能修改 `BattleUI/BattleController` 后，你再回头决定：
- 是继续保持“原项目框架版”作为最终提交
- 还是逐步把它重构回 ep4 的架构（那就不可能 1:1 了）

---

## 7) “最小战斗版”落地提示（跳过菜单，只看战斗界面）

目标：只要能看到战斗场景里的植物、僵尸、种植栏、背景音乐即可；菜单/选关可以不写。

建议做法：
- 在 `pvz2/game.cpp` 里直接进入战斗：初始化时创建 `UBattleUI` 与 `ABattleController`，并把当前 UI/Controller 指向它们（保持原项目接口调用方式，不要新增全局）。
- 资源路径仍依赖 `pvz2/res/`、`pvz2/engine/`，运行目录必须是 `ep4/pvz2/`，否则图片/音效会丢失。
- 音乐：沿用原项目中加载 BGM 的调用（通常在 `ABattleController::Init()` 或相关播放函数内），不要改函数名。
- 植物/僵尸/种植栏：照抄原项目 `BattleUI`/`BattleController` 里的逻辑，实现最小卡槽 + 下地 + 僵尸出生即可，接口保持 1:1。
- 如果需要临时屏蔽价格/冷却/关卡逻辑，只在原文件里用条件短路，不要删改接口或换文件名。
