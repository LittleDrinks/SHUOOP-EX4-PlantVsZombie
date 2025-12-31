#include "Animator.h"

void Animation::Load(std::string path, int row, int col, int frames,
                     double speed, bool looped) {
    // 1. 确保图片正确加载
    Resources::getInstance().Load(path, path);
    info.img = Resources::getInstance().getImage(path);

    // 2. 设置参数
    info.row = row;
    info.col = col;
    info.flag = 1;
    totalFrames = frames;
    interval = speed;
    loop = looped;
    reset();
}

void Animation::update() {
    if (!loop && finished) {
        return;
    }

    timer += DELTA_TIME;
    if (timer >= interval) {
        timer = 0;
        info.index++;
        if (info.index >= totalFrames) {
            if (loop) {
                info.index = 0;
            } else {
                info.index = totalFrames > 0 ? (totalFrames - 1) : 0;
                finished = true;
            }
        }
    }
}

void Animation::reset() {
    timer = 0.0;
    info.index = 0;
    finished = false;
}

const SpriteInfo& Animation::getSpriteInfo() const { return info; }

void Animator::addAnimation(std::string name, std::string path, int row,
                            int col, int frames, double speed, bool looped) {
    Animation anim;
    anim.Load(path, row, col, frames, speed, looped);
    animations[name] = anim;

    // 如果是第一个动画，设置为当前动画
    if (curAnim == nullptr) {
        curAnim = &animations[name];
    }
}

void Animator::play(std::string name) {
    if (animations.find(name) != animations.end()) {
        auto* next = &animations[name];
        if (curAnim != next) {
            curAnim = next;
            curAnim->reset();
        }
    }
}

void Animator::update() {
    if (curAnim) {
        curAnim->update();
        if (owner) {
            owner->setAniSource(curAnim->getSpriteInfo());
        }
    }
}

bool Animator::isCurrentFinished() const {
    return curAnim ? curAnim->isFinished() : true;
}