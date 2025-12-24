#pragma once

// Timer 接口骨架（只定义接口，不提供实现）
//
// 目标：与参考项目 PlantVsZombie2 的 Timer 机制完全统一，行为要求如下：
// 1) 使用 timeGetTime() 获取当前时间（毫秒）。
// 2) 维护内部 lastTime，用于计算距离上次触发的间隔。
// 3) 构造时将 &lastTime 插入 Overall.h 的 gameLastTimes。
// 4) 析构时将 &lastTime 从 gameLastTimes 删除。
// 5) Bind(del, func, owner, flag)：
//    - del 单位为秒
//    - func 为成员函数指针：void(T::*)()
//    - owner 为对象指针
//    - flag=true 表示持续触发；flag=false 表示只触发一次后解绑
// 6) Tick()：每帧调用一次；到达 delay 后触发回调并按 flag 更新 lastTime/解绑。
// 7) Pause/Continue：Continue() 会遍历 gameLastTimes 对所有 lastTime
// 做时间补偿；
//    Timer 必须依赖该机制保证暂停期间不跳时。

#include <windows.h>

#include "Overall.h"

template <typename T>
class Timer {
public:
    // 构造：必须执行注册逻辑：gameLastTimes.insert(&lastTime)。
    Timer();

    // 析构：必须执行注销逻辑：gameLastTimes.erase(&lastTime)。
    ~Timer();

    // 绑定一个延迟触发任务。
    // 参数：
    // - del：延迟时间（秒）。内部应转换为毫秒 delay。
    // - func：回调成员函数指针，签名为 void(T::*)()。
    // - owner：回调所属对象指针。
    // - flag：true=持续触发（每次触发后重置
    // lastTime）；false=只触发一次（触发后解绑 owner）。
    void bind(double del, void (T::*func)(), T* owner, bool flag = false);

    // 每帧推进计时器。
    // 触发条件：getDelay() >= delay。
    // 触发后行为：
    // - 调用 (owner->*function)()。
    // - flag=true：lastTime = timeGetTime()。
    // - flag=false：pOwner 置空，后续 Tick 不再触发。
    void tick();

    // 设置触发间隔（秒）。内部应转换为毫秒 delay。
    void setDelay(double time);

    // 手动重置 lastTime：将 lastTime 设置为当前 timeGetTime()。
    void reset();

private:
    DWORD currentTime = 0;
    DWORD lastTime = 0;
    DWORD delay = 10000;
    bool bPersistant = false;
    void (T::*function)() = nullptr;
    T* pOwner = nullptr;

    // 返回距离 lastTime 的时间差（毫秒）。
    // 要求：当 lastTime 为 0 时必须初始化 lastTime 并返回 0。
    DWORD getDelay();
};

template <typename T>
Timer<T>::Timer() {
    gameLastTimes.insert(&lastTime);
}

template <typename T>
Timer<T>::~Timer() {
    gameLastTimes.erase(&lastTime);
}

template <typename T>
void Timer<T>::bind(double del, void (T::*func)(), T* owner, bool flag) {
    delay = static_cast<DWORD>(del * 1000);
    function = func;
    pOwner = owner;
    bPersistant = flag;
}

template <typename T>
void Timer<T>::tick() {
    if (pOwner == nullptr) {
        return;
    }
    if (getDelay() >= delay) {
        (pOwner->*function)();
        if (bPersistant) {
            lastTime = timeGetTime();
        } else {
            pOwner = nullptr;
        }
    }
}

template <typename T>
void Timer<T>::setDelay(double time) {
    delay = static_cast<DWORD>(time * 1000);
}

template <typename T>
void Timer<T>::reset() {
    lastTime = timeGetTime();
}

template <typename T>
DWORD Timer<T>::getDelay() {
    currentTime = timeGetTime();
    if (lastTime == 0) {
        lastTime = currentTime;
        return 0;
    } else {
        return currentTime - lastTime;
    }
}