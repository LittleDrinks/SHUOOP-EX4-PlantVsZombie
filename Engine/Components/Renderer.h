#pragma once
#include "../coreMinimal.h"

// SpriteRenderer：精灵渲染组件。
// 职责：
// - update(): 可选地从 owner->getAniSource() 拉取当前帧 SpriteInfo。
// - Render(): 把 sprite.img 按 row/col/index 裁剪后绘制到屏幕。
//
// 约束：
// - sprite.img 必须指向有效 IMAGE；否则 Render() 不绘制。
// - Render() 由引擎主循环遍历 GameRenders 调用。
class SpriteRenderer : public VisualComponent {
private:
    // 是否从 Object::aniSource 获取动画帧信息。
    // true：update() 每帧用 owner->getAniSource() 覆盖 sprite。
    // false：sprite 保持为 load()/setImage() 的静态值。
    bool bFecthAni = true;
public:
    SpriteRenderer() = default;
    virtual ~SpriteRenderer() = default;

    // 每帧更新。
    // 调用时机：由主循环遍历 GameComponents_ 调用。
    virtual void update() override;

    // 加载图片并设置到 sprite.img。
    // 参数：path 同时作为资源名与文件路径使用。
    // 副作用：触发 Resources::Load 并更新 sprite.img。
    void load(std::string path);

    // 设置是否拉取 owner->aniSource。
    void setFetchAni(bool fetch);

    // 绘制当前 sprite。
    // 调用时机：由主循环遍历 GameRenders 调用。
    void Render() override;
};

// StaticMesh：带默认 SpriteRenderer 的对象基类。
// 职责：
// - 构造时创建 SpriteRenderer 组件，并 attach 到 root。
// - 为派生类提供 getRenderer() 访问渲染器。
//
// 约束：
// - renderer 由 GameStatic::createComponent 创建并注册到 GameComponents_。
class StaticMesh : public Object {
protected:
    SpriteRenderer* renderer = nullptr;

public:
    StaticMesh();
    virtual void update() { }

    // 获取渲染组件。
    // 返回值为 VisualComponent*，便于按组件系统统一处理。
    VisualComponent* getRenderer() const;
};