#pragma once

// 启动游戏引擎并进入主循环。
// 行为：
// - 初始化图形窗口（initgraph/BeginBatchDraw 等）。
// - 创建初始对象（背景、僵尸、植物）。
// - 进入 while(true) 主循环：输入 -> 更新 -> 碰撞 -> 渲染。
// 约束：该函数不返回（死循环）。
void StartGame();
