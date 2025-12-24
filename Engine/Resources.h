#pragma once
#include "coreMinimal.h"
#include <map>
#include <string>

// Resources：资源池（图片与路径映射）。
// 约束：
// - imagePool 内的 IMAGE* 由 Resources 创建并持有所有权。
// - Load(name, ...) 对同名资源是幂等行为：已存在则不重复创建。
// - getImage(name) 在资源不存在时返回 nullptr。
class Resources
{
private:
    std::map<std::string, IMAGE*> imagePool;
    std::map<std::string, std::string> pathPool;
public:
    // 获取 Resources 单例引用。
    // 调用时机：任何需要加载/获取资源的地方都可调用。
    static Resources& getInstance();

    // 初始化资源池。
    // 约束：当前实现为空实现；调用不会修改资源池状态。
    void init();

    // 加载图片资源到资源池。
    // 参数：
    // - name：资源键（后续通过 getImage(name) 获取）。
    // - path：磁盘路径（传给 EasyX loadimage）。
    // - w/h：目标宽高（像素）。传 0 表示按原图尺寸加载。
    void Load(std::string name, std::string path, int w, int h);

    // 加载多媒体文件或按默认尺寸加载图片。
    // 当前实现：等价于 Load(name, path, 0, 0)。
    void Load(std::string name, std::string path);  // 加载多媒体文件

    // 通过 name 获取图片指针。
    // 返回：
    // - 找到：返回 IMAGE*（由 Resources 持有，不要 delete）。
    // - 未找到：返回 nullptr。
    IMAGE* getImage(std::string name); // Alias for fetch

    // 通过 name 获取路径字符串。
    // 返回：
    // - 找到：返回原始 path。
    // - 未找到：返回空字符串。
    std::string findPath(std::string name);
};