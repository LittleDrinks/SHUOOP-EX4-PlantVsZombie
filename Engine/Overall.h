#pragma once
#include <set>
#include <graphics.h>

// 窗口宽度（像素）。由引擎启动时 initgraph(WIN_WIDTH, WIN_HEIGHT) 使用。
extern const int WIN_WIDTH;
// 窗口高度（像素）。由引擎启动时 initgraph(WIN_WIDTH, WIN_HEIGHT) 使用。
extern const int WIN_HEIGHT;

// 固定时间步长（秒）。主循环会用它做帧率门限：elapsed < DELTA_TIME 时跳过更新。
extern const double DELTA_TIME;

// Timer 系统注册表：保存所有计时器内部 lastTime 的地址。
// Continue() 会遍历该集合，对每个 *lastTime 做暂停补偿（加上暂停时长）。
// 约束：集合内元素必须是有效指针；Timer 析构必须负责注销。
extern std::set<DWORD*> gameLastTimes;
// 最近一次调用 Pause() 的时刻（timeGetTime() 毫秒值）。Continue() 通过它计算暂停时长。
extern DWORD pauseTime;
// 暂停瞬间截取的屏幕图像（用于暂停界面/恢复前显示）。由 Pause() 写入。
extern class IMAGE pauseImage;
// 暂停开关：Pause() 置 true；Continue() 置 false。
extern bool pause;

class Resources;
// 全局资源池引用。
// 要求：在程序生命周期内保持可用；引用指向 Resources 单例。
extern Resources& ResourcePool;
