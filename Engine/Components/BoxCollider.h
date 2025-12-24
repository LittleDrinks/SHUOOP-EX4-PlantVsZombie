#pragma once
#include <string>
#include <vector>
#include <set>

#include "../coreMinimal.h"

// Collider：碰撞器基类。
// 职责：提供统一的碰撞检测接口与碰撞回调。
// 调用路径：主循环 -> CollisionJudge() -> checkCollision()/onCollision().
class Collider : public Component {
public:
    Collider() {}

    // 检测与另一个碰撞器是否发生碰撞。
    // 返回：true 表示发生碰撞；false 表示未碰撞。
    virtual bool checkCollision(Collider* other) = 0;

    // 碰撞回调。
    // 触发条件：CollisionJudge() 判定 checkCollision(other)==true 后调用。
    // 默认实现为空；派生类通过重写实现游戏逻辑响应。
    virtual void onCollision(Collider* other) {}
};

// BoxCollider：AABB 盒碰撞体。
// 语义：
// - size 表示碰撞箱尺寸（像素）。
// - 碰撞判定使用轴对齐包围盒（AABB）。
//
// 生命周期：
// - 构造时自动注册到全局集合 GameColliders_。
// - 析构时自动从 GameColliders_ 注销。
class BoxCollider : public Collider {
public:
    Point size;  // 盒子大小

private:
    // 碰撞类型标签：用于在游戏逻辑中按“目标类别”查询碰撞结果。
    // 例："Plant"、"Zombie"、"Bullet"。
    std::string type = "Default";

    // 是否参与碰撞插入与查询。
    // 说明：关闭后仍可被 checkCollision 调用，但不会被 Insert() 写入碰撞集合。
    bool open = true;

    // 当前帧的碰撞集合（由 CollisionJudge() 填充）。
    std::set<BoxCollider*> boxes;

    // GetCollisions() 的复用输出缓冲区（避免频繁分配）。
    std::vector<Object*> aims;

public:
    BoxCollider(const Point& sz = Point(50, 50));
    ~BoxCollider();

    void setSize(const Point& sz);
    Point getSize() const { return size; }

    const std::string& getType() const { return type; }
    void setType(const std::string& t) { type = t; }

    void setOpen(bool enabled) { open = enabled; }
    bool getOpen() const { return open; }

    void clear();
    void Insert(BoxCollider* another);
    const std::vector<Object*>& getCollisions(const std::string& t);

    bool checkCollision(Collider* other) override;
};

// 全局碰撞体注册表。
// CollisionJudge() 遍历该集合做两两碰撞检测。
extern std::set<BoxCollider*> GameColliders_;