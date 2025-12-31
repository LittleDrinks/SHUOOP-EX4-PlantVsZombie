#pragma once

#include <map>
#include <string>

#include "../Resources.h"
#include "../coreMinimal.h"

// Animation：单个动画片段的运行时状态。
// 职责：
// - 持有一张 sprite sheet（info.img）及其行列信息（info.row/info.col）。
// - 维护当前帧下标（info.index）。
// - update() 按 interval 推进帧下标。
//
// 约束：
// - interval 单位为秒。
// - totalFrames 表示可播放的帧数上限；info.index 的范围必须保持在 [0, totalFrames)。
class Animation {
private:
    SpriteInfo info;
    int totalFrames = 0;    // 总帧数
    double interval = 0.1;  // 帧间隔，单位秒
    double timer = 0.0;     // 计时器
    bool loop = true;       // 是否循环播放
    bool finished = false;  // 非循环播放时：是否已播到末尾

public:
    Animation() {}

    // 加载动画资源并初始化播放参数。
    // 参数：
    // - path：资源路径；同时作为资源池 key 使用。
    // - row/col：sprite sheet 的行/列。
    // - frames：有效帧数（用于限制 index）。
    // - speed：帧间隔（秒）。
    // - looped：是否循环播放。
    // 副作用：调用 Resources::Load 并设置 info.img。
    void Load(std::string path, int row, int col, int frames,
              double speed, bool looped = true);

    // 推进动画：根据 interval 更新 info.index。
    // 调用时机：由 Animator::update() 每帧调用。
    void update();

    // 重置动画到第 0 帧。
    void reset();

    // 是否播完（仅对非循环动画有意义）。
    bool isFinished() const { return finished; }

    // 获取当前帧的 SpriteInfo。
    // 用途：Animator 把该值写入 owner->setAniSource(...)，供 SpriteRenderer 裁剪渲染。
    const SpriteInfo& getSpriteInfo() const;
};

// Animator：动画控制器组件。
// 职责：
// - 管理多个 Animation（状态机的资源集合）。
// - play(name) 选择当前动画。
// - update() 每帧推进当前动画，并将结果写入 owner->aniSource。
//
// 约束：
// - Animator 必须 attach 到某个 Object 的 root/组件树上，owner 必须非空。
class Animator : public Component {
private:
    std::map<std::string, Animation> animations;  // 动画库
    Animation* curAnim = nullptr;                   // 当前动画

public:
    Animator() {}

    // 添加一个动画到动画库。
    // 参数：
    // - name：动画名（play() 的 key）。
    // - path/row/col/frames/speed：见 Animation::Load。
    void addAnimation(std::string name, std::string path, int row, int col, int frames, double speed, bool looped = true);

    // 切换当前动画。
    // 行为：如果 name 存在，则 curAnim 指向 animations[name]。
    void play(std::string name);

    // 每帧更新：推进动画并写入 owner->setAniSource。
    void update() override;

    // 当前动画是否播放完（仅对非循环动画有意义）。
    bool isCurrentFinished() const;
};