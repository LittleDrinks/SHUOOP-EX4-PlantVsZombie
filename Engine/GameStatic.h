#pragma once

#include "Objects/Controller.h"
#include "Objects/GameInstance.h"
#include "coreMinimal.h"


// GameStatic：引擎级静态工具类。
// 主要职责：
// 1) 统一通过 new 创建 Object/Component。
// 2) 把新建实例注册到全局集合（GameObjects_ / GameComponents_）。
// 3) 提供全局唯一的玩家 Controller（惰性创建）。
//
// 约束：
// - createObject/createComponent 返回裸指针；实例的生命周期由引擎注册表追踪。
// - 不允许把非 Object/Component 类型传给模板；否则会创建失败并返回 nullptr。
class GameStatic {
private:
    static Controller* playerController;

public:
    // 创建游戏对象并注册到 GameObjects_。
    // 参数：
    // - pos：对象初始位置（世界坐标，像素单位）。
    // 返回：
    // - 成功：返回新对象指针。
    // - 失败：返回 nullptr，并输出错误信息。
    template <typename T>
    static T* createObject(Point pos) {
        T* obj = new T{};
        if (Object* buffer = static_cast<Object*>(obj)) {
            buffer->setPosition(pos);
            GameObjects.insert(buffer);
        } else {
            delete obj;
            obj = nullptr;
            std::cerr << "fail to create Object !" << std::endl;
        }
        return obj;
    }

    // 创建组件并注册到 GameComponents_。
    // 参数：
    // - pos：组件初始位置（局部/世界坐标由挂载关系决定；未挂载前按局部使用）。
    // 返回：
    // - 成功：返回新组件指针。
    // - 失败：返回 nullptr，并输出错误信息。
    template <typename T>
    static T* createComponent(Point pos) {
        T* comp = new T{};
        if (Component* buffer = static_cast<Component*>(comp)) {
            buffer->setPosition(pos);
            GameComponents.insert(buffer);
        } else {
            delete comp;
            comp = nullptr;
            std::cerr << "fail to create component !" << std::endl;
        }
        return comp;
    }

    // 获取玩家控制器（Controller 单例）。
    // 行为：首次调用时 new Controller()；后续返回同一指针。
    // 调用时机：主循环每帧处理输入前调用。
    static Controller* getPlayerController();
};
