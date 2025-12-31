#pragma once

// 图形渲染
#include <easyx.h>
#include <winuser.h>
#pragma comment(lib, "MSIMG32.LIB")  // 背景透明需要链接此库

// 多媒体
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")  // 多媒体需要链接此库

// 其他
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <vector>
#include <string>

#include "Overall.h"  // 全局配置

const double PI = acos(-1.0);  // 圆周率

/*
 * classes
 *    1. Point
 *    2. Transform
 *    3. SpriteInfo
 *    4. SpriteInfo_
 *    5. Component
 *    6. VisualComponent
 *    7. Object
 */

// Point：二维坐标/向量。
// 坐标系：窗口/世界坐标（像素）。
// 用途：位置、尺寸、速度、偏移等统一使用该类型。
struct Point {
    double x;
    double y;
    // 构造：默认 (0,0)。
    Point(double _x = 0.0, double _y = 0.0);

    // 计算两点欧氏距离。
    static double Dist(const Point& a, const Point& b);

    // 比较/运算符：用于 set 排序、位置运算、缩放等。
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    bool operator<(const Point& other) const;
    Point operator+(const Point& other) const;
    Point& operator+=(const Point& other);
    Point operator-(const Point& other) const;
    Point& operator-=(const Point& other);
    Point operator*(double scalar) const;
    Point& operator*=(double scalar);
    friend Point operator*(double scalar, const Point& v);
    double operator*(const Point& other) const;
    friend std::istream& operator>>(std::istream& in, Point& v);
    friend std::ostream& operator<<(std::ostream& out, const Point& v);
};

// Transform：组件的局部变换。
// rotation 单位：弧度。
struct Transform {
    Point position;   // 位置
    double rotation;  // 旋转，弧度制
    Transform(const Point& pos = Point(), double rot = 0.0);
};

// SpriteInfo：渲染输入数据。
// 约束：
// - img 指向有效 IMAGE 时才可绘制。
// - row/col/index 用于 sprite sheet 裁剪。
// - flag 用于标记是否启用动画信息通道。
struct SpriteInfo {
    IMAGE* img = nullptr;  // 图像指针
    std::string name = "";

    // 资源位置偏移
    //  1.解决同一渲染器渲染不同动画资源位置偏差问题
    //  2.解决非sprite图旋转后中心坐标偏移问题
    Point delta = Point();

    // 动画标记
    // 0 : 无动画
    // 1 : 整张动画资源
    // 2 : 分开动画资源
    int flag = 0;

    // 以下信息为一整张 sprite 类资源独有
    int row = 1;
    int col = 1;
    int index = 0;
};
// SpriteInfo_：渲染附加参数（目标尺寸/位置等）。
// 语义：当 isUsed=true 时，渲染器使用 aimSize/aimLoc 作为固定绘制矩形。
struct SpriteInfo_ {
    bool isUsed = false;
    Point aimSize = Point(0, 0);
    Point aimLoc = Point(0, 0);
};

// Component：组件基类（逻辑/变换/挂载）。
// 生命周期与注册：
// - 构造时自动注册到全局集合 GameComponents_。
// - Destruct()/析构会从 GameComponents_ 注销。
// - setAttachment() 负责建立父子组件关系。
// 调用时机：主循环每帧遍历 GameComponents_ 调用 update()。
class Component {
private:
    friend class Object;
    void setOwner(class Object* obj);

protected:
    Object* owner = nullptr;
    Transform transform;
    Component* parent = nullptr;
    std::set<Component*> sons;
    void processDestruction();

public:
    Component();
    virtual ~Component();

    // 每帧更新。
    // 默认实现：同步挂载关系与变换；派生类可 override 并在开头调用 Component::update()。
    virtual void update();

    // 获取组件所属 Object。
    class Object* getOwner() const;

    // 获取局部 Transform（位置/旋转）。
    Transform getTransform() const;

    // 将当前组件挂载到父组件 p 下。
    // 副作用：建立 parent/sons 关系，并影响世界变换计算。
    void setAttachment(Component* p);

    // 销毁组件：从父子树脱离并触发回收流程。
    void Destruct();

    // 变换查询：世界坐标由父链叠加得到。
    Point getWorldPosition();
    double getWorldRotation();
    Point getLocalPosition();
    double getLocalRotation();

    // 变换设置：修改局部 Transform。
    void setPosition(Point pos);
    void setRotation(double rot);
    void addPosition(Point delta);
    void addRotation(double delta);
};

// VisualComponent：可渲染组件基类。
// 注册：构造时插入 GameRenders；析构时移除。
// Render()：纯虚函数，由具体渲染组件实现。
class VisualComponent : public Component {
private:
    // 临时寄存变量
    struct TempInfo {
        double angle = 0;
        COLORREF color = BLACK;
    } temp;
    // 对性能消耗极大，出动画图以外都支持
    IMAGE rotateImage(IMAGE* pImg, double rad);
    void FilterImage();

protected:
    SpriteInfo sprite;    // 渲染信息
    SpriteInfo_ sprite_;  // 渲染补充

    int layer = 0;            // 渲染层级
    BYTE transparency = 255;  // 透明度 0~255

    IMAGE* en = nullptr;  // 掩码图（仅用于静态图）
    bool bEn = false;     // 使用掩码图

    IMAGE* copy = nullptr;  // 图像备份（便于调整图像大小、旋转或颜色滤镜）

    bool filter = false;  // 是否开启滤镜

public:
    VisualComponent();
    ~VisualComponent();
    virtual void update();

    // 加载静态图资源。
    // path：图片路径；path_en：掩码图路径（用于透明/抠图）。
    // w/h：目标尺寸（像素）。
    void Load_(std::string path, std::string path_en, int w, int h);

    // 设置缩放（仅用于静态图）。
    // scale 为比例值：例如 (2,2) 表示宽高放大 2 倍。
    virtual void setScale(Point scale);

    void setLayer(int l);
    int getLayer() const;

    void setTransparency(BYTE t);
    BYTE getTransparency() const;

    void setImage(IMAGE* img);

    void setFilter(bool open, COLORREF c = BLACK);

    void setFixEnable(bool en);
    void setFixSize(Point sz);
    void setFixLocation(Point loc);

    // 渲染入口：主循环遍历 GameRenders 调用。
    virtual void Render() = 0;
};

// Object：游戏对象基类。
// 语义：Object 持有一个 root Component 作为组件树根。
// 注册：构造/析构会维护全局集合 GameObjects_。
// update()：纯虚函数，由派生类实现具体逻辑。
class Object {
protected:
    Component* root;
    Object* parent = nullptr;
    std::set<Object*> sons;

    SpriteInfo aniSource;  // 动画源信息（用于记录并传递动画信息）

    void processDestruction();

public:
    Object();
    virtual ~Object();

    // 每帧更新。
    // 调用时机：主循环遍历 GameObjects 调用。
    virtual void update() = 0;

    // 动画源信息通道：Animator 写入；SpriteRenderer 读取。
    const SpriteInfo& getAniSource() const;
    void setAniSource(const SpriteInfo& info);

    void setParent(Object* par);

    void undoAttachment(Object* par);

    void Destroy();

    Point getWorldPosition();
    Point getLocalPosition();
    void setPosition(Point pos);
    void addPosition(Point delta);

    double getWorldRotation();
    double getLocalRotation();
    void setRotation(double rot);
    void addRotation(double delta);
};

// VisualComponent 排序比较：用于按 layer 渲染。
bool cmp(VisualComponent* a, VisualComponent* b);

// 全局渲染集合：元素为所有 VisualComponent，按 cmp 排序。
extern std::set<VisualComponent*, decltype(&cmp)> GameRenders;

// 全局组件集合：元素为所有 Component。
extern std::set<Component*> GameComponents;
extern std::set<Component*> DGameComponents;

// 全局对象集合：元素为所有 Object。
extern std::set<Object*> GameObjects;
extern std::set<Object*> DGameObjects;

// --- PVZ2-like UI globals (compat layer) ---
class UserInterface;
class Widget;
extern std::set<UserInterface*> GameUIs;
extern std::vector<UserInterface*> GameUIs_;
extern std::set<Widget*> GamePainters;

/*
 * functions
 *    1. RandInt
 *    2. RandRatio
 *    3. Cast
 *    4. Pause
 *    5. Continue
 *
 */

// 提供随机数支持
extern std::mt19937_64 rng;

// 生成闭区间 [l, r] 的整数随机数。
int RandInt(int l, int r);

// 生成 [0, 1] 的浮点随机数。
double RandRatio();

// 逐帧碰撞判定入口：遍历已注册的 BoxCollider，并触发 onCollision
void CollisionJudge();

// 类型转换
template <typename T>
static T* Cast(Object* comp) {
    return dynamic_cast<T*>(comp);
}
template <typename T>
static T* Cast(Component* comp) {
    return dynamic_cast<T*>(comp);
}

// 游戏暂停
void Pause();

// 取消暂停
void Continue();