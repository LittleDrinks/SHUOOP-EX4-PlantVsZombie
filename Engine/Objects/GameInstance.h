#pragma once

// GameInstance：全局游戏实例。
// 用途：放置跨场景/跨系统共享的状态（例如关卡信息、分数、全局配置）。
// 当前实现：仅提供构造函数声明；具体字段与逻辑由上层补充。
class GameInstance {
   public:
    GameInstance();
};

// 全局游戏实例指针。
// 约束：指针的创建与销毁策略由上层决定；当前代码库未提供统一的生命周期管理函数。
extern GameInstance* GlobalInstance;