# ep4 复现指南（完全对齐原版 PlantVsZombie2 框架）

目标：在 ep4 内部以**原项目的类名/接口/调用链**跑出最小战斗界面（植物、僵尸、种植栏、背景音乐），不写菜单，不发明任何新接口/全局名。

适用范围：
- 你想要“文件名一致、接口名一致、调用位置一致”。
- 不接受 `Pvz2CompatGlobals` 等自创命名，也不接受在 ep4 Engine 上套一层兼容层。

验收标准：
1) 新增的可执行目标 `pvz2_main` 能编译运行。
2) 运行时工作目录指向 `ep4/pvz2/`，资源加载正常（图片/音频不缺）。
3) 一启动就进入战斗：能看到草坪、种植栏、至少一种植物能下地，至少一种僵尸能出现，背景音乐播放。
4) 所有类/函数/全局名与原项目完全一致（如 `GameStatics`、`Controller`、`UserInterface`、`UBattleUI`、`ABattleController`），无自创命名。

---

## 1) 目录与资源：直接搬原项目

1. 在 `ep4/` 下创建 `pvz2/`。
2. 从原项目复制到 `ep4/pvz2/`：
   - `PlantVsZombie2/*.h`、`PlantVsZombie2/*.cpp`
   - `PlantVsZombie2/engine/`
   - `PlantVsZombie2/res/`
3. 不要复制生成物（`x64/`、`.sln`、`.vcxproj`、可执行文件）。
4. 运行时工作目录必须是 `ep4/pvz2/`，否则资源全挂。

---

## 2) CMake：新增原版可执行 pvz2_main

在 `ep4/CMakeLists.txt` 末尾追加：

```cmake
# --- 原项目框架版（PlantVsZombie2） ---
file(GLOB_RECURSE PVZ2_SRC
    pvz2/**.cpp
    pvz2/**.h
)

add_executable(pvz2_main ${PVZ2_SRC})
target_link_libraries(pvz2_main libeasyx.a winmm Msimg32)
```

注意：
- 确保 `pvz2_main` 只编译 `pvz2/**`，不要把 `src/main.cpp` 混进来。
- 如果链接报“找不到 main/WinMain”，把 `pvz2/game.cpp` 末尾 `void main()` 改成 `int main()` 并 `return 0;`，其他不动。

---

## 3) 运行方式（工作目录必须对）

- VS Code/CMake Tools：选择目标 `pvz2_main`，运行配置的 `cwd` 设为 `ep4/pvz2`。
- 命令行：先 `cd ep4/pvz2`，再从构建输出目录运行 `pvz2_main.exe`。

资源缺失、全白屏、无音乐，十有八九是工作目录错了。

---

## 4) 最小战斗版该改哪些文件（且只改这些）

- `pvz2/BattleUI.h`、`pvz2/BattleUI.cpp`：负责选卡/种植栏，产出 `index`、`failFlag` 等原版字段。
- `pvz2/BattleController.h`、`pvz2/BattleController.cpp`：消费 `index`，生成/拖拽/落子植物，生成僵尸，播放 BGM，调用 `FindCoordinate()`（保持公式不改）。
- `pvz2/game.cpp`：入口直接进入战斗，不写菜单。保持原版接口，创建 `UBattleUI` 和 `ABattleController`，把当前 UI/Controller 指向它们（用原项目已有的全局/接口，不新增自创指针）。

禁止事项：
- 不要新增任何兼容层命名（例如 `Pvz2CompatGlobals`、`MainController` 指针别名等）。
- 不要改类/函数签名；业务逻辑可以简化，但接口名必须与原项目一致。

简化建议（仍在原文件内完成）：
- 卡槽可以先做最少数量（比如 1-2 张），价格/冷却可临时放宽逻辑，但接口字段保留。
- 僵尸种类可先放 1 种，生成点/波次可写死。
- BGM：沿用原项目播放调用（通常在 `ABattleController::Init()` 或对应函数中），不要换接口名。

---

## 5) 常见踩坑

- 资源加载失败：工作目录没指到 `ep4/pvz2/`。
- 链接报 main/WinMain：按上面的最小改法把 `void main()` 改成 `int main()`。
- easyx/AlphaBlend/winmm 链接失败：`pvz2_main` 目标缺少 `libeasyx.a winmm Msimg32`。
- 发现自创命名：回退，删除自创的全局/接口，直接用原项目的类/函数。

---

## 6) 验收清单（最小战斗版）

运行 `pvz2_main` 后应看到：
- 草坪、种植栏、至少一张卡可选，鼠标可拖放植物到草坪上。
- 至少一种僵尸会出现并移动。
- 背景音乐正常播放。
- 使用的类/函数/全局名全部来自原项目，无新增命名。

满足以上即可过本轮，先跑通再扩展功能。 

