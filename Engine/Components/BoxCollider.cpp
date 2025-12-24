#include <cassert>

#include "BoxCollider.h"


// 定义全局碰撞体容器
std::set<BoxCollider*> GameColliders_;

BoxCollider::BoxCollider(const Point& sz) : size(sz) {
    GameColliders_.insert(this);  // 自动注册
}

BoxCollider::~BoxCollider() {
    // 关键：本碰撞体析构时，其他碰撞体的 boxes 里可能仍保存着 this。
    // 下一帧它们在 update() 里调用 getCollisions() 时会解引用悬垂指针导致崩溃。
    // 因此这里需要把 this 从所有存活碰撞体的 boxes 中剔除。
    for (auto* c : GameColliders_) {
        if (!c || c == this) continue;
        c->boxes.erase(this);
    }
    GameColliders_.erase(this);  // 自动注销
}

void BoxCollider::setSize(const Point& sz) { size = sz; }

void BoxCollider::clear() { boxes.clear(); }

void BoxCollider::Insert(BoxCollider* another) {
    if (!open || !another || !another->getOpen()) {
        return;
    }
    if (boxes.find(another) != boxes.end()) {
        return;
    }
    if (checkCollision(another)) {
        boxes.insert(another);
    }
}

const std::vector<Object*>& BoxCollider::getCollisions(const std::string& t) {
    aims.clear();
    if (!open) {
        return aims;
    }
    // 注意：boxes 里可能残留已析构的 BoxCollider*（例如上一帧发生碰撞、
    // 本帧开始时子弹被 Destroy 并删除）。此处需要先剔除陈旧指针再访问其成员。
    for (auto it = boxes.begin(); it != boxes.end();) {
        BoxCollider* c = *it;
        if (!c || GameColliders_.find(c) == GameColliders_.end()) {
            it = boxes.erase(it);
            continue;
        }
        if (c->getType() == t) {
            if (auto* owner = c->getOwner()) {
                aims.push_back(owner);
            }
        }
        ++it;
    }
    return aims;
}

bool BoxCollider::checkCollision(Collider* other) {
    BoxCollider* box = dynamic_cast<BoxCollider*>(other);
    if (!box) {
        return false;
    }

    // 与渲染坐标一致：getWorldPosition() 视为左上角坐标。
    Point a = getWorldPosition();
    Point b = box->getWorldPosition();

    return (a.x < b.x + box->size.x) && (a.x + size.x > b.x) &&
           (a.y < b.y + box->size.y) && (a.y + size.y > b.y);
}