#include "Components/BoxCollider.h"
#include "Resources.h"  // 资源管理
#include "coreMinimal.h"

/*
 * 全局变量
 */
std::set<Object*> GameObjects;
std::set<Component*> GameComponents;
std::set<Object*> DGameObjects;
std::set<Component*> DGameComponents;

/*
 * Point
 */
Point::Point(double _x, double _y) : x(_x), y(_y) {}

double Point::Dist(const Point& a, const Point& b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

bool Point::operator==(const Point& other) const {
    return x == other.x && y == other.y;
}

bool Point::operator!=(const Point& other) const { return !(*this == other); }

bool Point::operator<(const Point& other) const {
    return x < other.x || (x == other.x && y < other.y);
}

Point Point::operator+(const Point& other) const {
    return Point(x + other.x, y + other.y);
}

Point& Point::operator+=(const Point& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Point Point::operator-(const Point& other) const {
    return Point(x - other.x, y - other.y);
}

Point& Point::operator-=(const Point& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Point Point::operator*(double scalar) const {
    return Point(x * scalar, y * scalar);
}

Point& Point::operator*=(double scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Point operator*(double scalar, const Point& v) {
    return Point(v.x * scalar, v.y * scalar);
}

double Point::operator*(const Point& other) const {
    return x * other.x + y * other.y;
}

std::istream& operator>>(std::istream& in, Point& v) {
    return in >> v.x >> v.y;
}

std::ostream& operator<<(std::ostream& out, const Point& v) {
    return out << v.x << " " << v.y;
}

/*
 * Transform
 */
Transform::Transform(const Point& pos, double rot)
    : position(pos), rotation(rot) {}

/*
 * Component implementation
 */
void Component::setOwner(class Object* obj) {
    if (!owner) {
        owner = obj;
    } else {
        std::cerr << "Please use SetAttachment instead" << std::endl;
    }
}

void Component::processDestruction() {
    if (!sons.empty()) {
        for (auto& s : sons) {
            s->processDestruction();
        }
    }
    DGameComponents.insert(this);
}

Component::Component() { GameComponents.insert(this); }

Component::~Component() {}

void Component::update() {}

Object* Component::getOwner() const { return owner; }

Transform Component::getTransform() const { return transform; }

void Component::setAttachment(Component* par) {
    parent = par;
    if (parent) {
        parent->sons.insert(this);

        Component* top = parent;
        while (top->parent) {
            top = top->parent;
        }
        owner = top->owner;
    }
}

void Component::Destruct() {
    if (parent) {
        parent->sons.erase(this);
    }
    processDestruction();
}

Point Component::getWorldPosition() {
    if (parent) {
        return parent->getWorldPosition() + getLocalPosition();
    } else {
        if (owner) {
            return owner->getWorldPosition();
        } else {
            return getLocalPosition();
        }
    }
}

double Component::getWorldRotation() {
    if (parent) {
        return parent->getWorldRotation() + getLocalRotation();
    } else {
        if (owner) {
            return owner->getWorldRotation();
        } else {
            return getLocalRotation();
        }
    }
}

Point Component::getLocalPosition() { return transform.position; }
double Component::getLocalRotation() { return transform.rotation; }
void Component::setPosition(Point pos) { transform.position = pos; }
void Component::setRotation(double rot) { transform.rotation = rot; }
void Component::addPosition(Point delta) { transform.position += delta; }
void Component::addRotation(double delta) { transform.rotation += delta; }

/*
 * VisualComponent
 */
bool cmp(VisualComponent* a, VisualComponent* b) {
    if (a->getLayer() != b->getLayer()) {
        return a->getLayer() < b->getLayer();
    } else {
        return a < b;
    }
}
std::set<VisualComponent*, decltype(&cmp)> GameRenders(&cmp);

IMAGE VisualComponent::rotateImage(IMAGE* pImg, double rad) {
    // TODO: 实现图像旋转
    if (pImg) return *pImg;
    return IMAGE();
}

void VisualComponent::FilterImage() {
    // TODO
}

VisualComponent::VisualComponent() { GameRenders.insert(this); }

VisualComponent::~VisualComponent() {
    GameRenders.erase(this);
    if (en) {
        delete en;
        en = nullptr;
    }
    if (copy) {
        delete copy;
        copy = nullptr;
    }
}

void VisualComponent::update() {
    double rot = getWorldRotation();
    if (rot != temp.angle) {
        if (copy) {
            delete copy;
            copy = nullptr;
        }
        copy = new IMAGE(rotateImage(sprite.img, PI * rot / 180.0));
        temp.angle = rot;
    }
}

void VisualComponent::Load_(std::string path, std::string path_en, int w,
                            int h) {
    // 本地临时图像（主图）缓冲区
    IMAGE buffer;

    // 如果已存在掩模图像指针 en，先释放旧对象避免内存泄漏
    if (en) {
        delete en;
        en = nullptr;
    }
    // 为掩模图像分配新的 IMAGE 对象（存放 path_en 对应的图像）
    en = new IMAGE();

    // 从文件加载主图到 buffer（注意：这里只传了 w，可能应传 w,h）
    loadimage(&buffer, path.c_str(), w);
    // 从文件加载掩模图像到 en（注意：这里只传了 h，可能应传 w,h）
    loadimage(en, path_en.c_str(), h);

    // 标记已启用掩模/增强图层
    bEn = true;

    // 调整 sprite 的目标图像到目标大小（用于离屏绘制）
    sprite.img->Resize(w, h);
    // 切换绘制目标：后续绘图操作将绘制到 sprite.img（离屏），不是直接到屏幕
    SetWorkingImage(sprite.img);

    // 第一步：把掩模图像 en 按 NOTSRCCOPY（取反复制）绘制到目标上
    // 这通常用于准备“遮罩”以实现透明效果（创建需要被清空的区域）
    putimage(0, 0, en, NOTSRCCOPY);

    // 第二步：把主图 buffer 以 SRCPAINT（按位 OR）方式绘制到目标上
    // 结合上一步的掩模，可实现带“透明/镂空”效果的合成绘制
    putimage(0, 0, &buffer, SRCPAINT);

    // 恢复默认绘制目标（回到屏幕或之前的工作图）
    SetWorkingImage();
}

void VisualComponent::setScale(Point scale) {
    if (copy) {
        delete copy;
        copy = nullptr;
    }
    loadimage(copy, ResourcePool.findPath(sprite.name).c_str(), (int)scale.x,
              (int)scale.y);
    sprite.img = copy;
}

void VisualComponent::setLayer(int l) {
    GameRenders.erase(this);
    layer = l;
    GameRenders.insert(this);
}
int VisualComponent::getLayer() const { return layer; }
void VisualComponent::setTransparency(BYTE t) { transparency = t; }
BYTE VisualComponent::getTransparency() const { return transparency; }

void VisualComponent::setFilter(bool open, COLORREF c) {
    filter = open;
    if (!open) {
        return;
    }
    if (copy) {
        delete copy;
        copy = nullptr;
    }
    copy = new IMAGE(*sprite.img);
    temp.color = c;
    FilterImage();
}

void VisualComponent::setFixEnable(bool en) { sprite_.isUsed = en; }
void VisualComponent::setFixSize(Point sz) { sprite_.aimSize = sz; }
void VisualComponent::setFixLocation(Point loc) { sprite_.aimLoc = loc; }

/*
 * Object
 */
void Object::processDestruction() {
    if (!sons.empty()) {
        for (auto& s : sons) {
            s->processDestruction();
        }
    }
    DGameObjects.insert(this);
}

Object::Object() {
    GameObjects.insert(this);
    root = new Component();
    root->setOwner(this);
}

Object::~Object() { root->Destruct(); }

const SpriteInfo& Object::getAniSource() const { return aniSource; }
void Object::setAniSource(const SpriteInfo& info) { aniSource = info; }

void Object::setParent(Object* par) {
    if (parent) {
        parent = par;
        parent->sons.insert(this);
    }
}

void Object::undoAttachment(Object* par) {
    setPosition(getWorldPosition());
    setRotation(getWorldRotation());
    parent = nullptr;
    par->sons.erase(this);
}

void Object::Destroy() {
    if (parent) {
        parent->sons.erase(this);
    }
    processDestruction();
}

Point Object::getWorldPosition() {
    if (parent) {
        return parent->getWorldPosition() + getLocalPosition();
    } else {
        return getLocalPosition();
    }
}

Point Object::getLocalPosition() { return root->getLocalPosition(); }
void Object::setPosition(Point pos) { root->setPosition(pos); }
void Object::addPosition(Point delta) { root->addPosition(delta); }

double Object::getWorldRotation() {
    if (parent) {
        return parent->getWorldRotation() + getLocalRotation();
    } else {
        return getLocalRotation();
    }
}

double Object::getLocalRotation() { return root->getLocalRotation(); }
void Object::setRotation(double rot) { root->setRotation(rot); }
void Object::addRotation(double delta) { root->addRotation(delta); }

/*
 * Functions
 */
std::mt19937_64 rng(
    std::chrono::steady_clock::now().time_since_epoch().count());

int RandInt(int l, int r) {
    std::uniform_int_distribution<int> dist(l, r);
    return dist(rng);
}

double RandRatio() {
    std::uniform_real_distribution<double> dist(0, 1);
    return dist(rng);
}

void Pause() {
    pauseTime = timeGetTime();
    pause = true;
    getimage(&pauseImage, 0, 0, WIN_WIDTH, WIN_HEIGHT);
}

void Continue() {
    DWORD now = timeGetTime();
    DWORD diff = now - pauseTime;
    for (auto p : gameLastTimes) {
        if (p) *p += diff;
    }
    pause = false;
}

void CollisionJudge() {
    // 避免在这里 include BoxCollider.h 造成循环依赖：使用前置声明的类型即可。
    // coreMinimal.h 已经包含 Overall/基础类型；碰撞体集合在 BoxCollider.cpp
    // 中定义。

    // 每帧开始先清空碰撞结果，保证 GetCollisions() 只反映“当前帧”。
    for (auto* c : GameColliders_) {
        if (c) c->clear();
    }

    // 注意：这里采用朴素 O(N^2) 两两检测。
    // 若后续需要分区加速，可在此函数内部替换实现，不影响外部接口。
    for (auto itA = GameColliders_.begin(); itA != GameColliders_.end();
         ++itA) {
        auto itB = itA;
        ++itB;
        for (; itB != GameColliders_.end(); ++itB) {
            BoxCollider* a = *itA;
            BoxCollider* b = *itB;
            if (!a || !b) continue;
            if (a->checkCollision(b)) {
                // 参考项目风格：把碰撞结果写入各自的碰撞集合，供对象在
                // Update/Judge 中查询。
                a->Insert(b);
                b->Insert(a);

                a->onCollision(b);
                b->onCollision(a);
            }
        }
    }
}
