#include "Animator.h"

void Animation::Load(std::string path, int row, int col, int frames,
                     double speed) {
    // 1. 确保图片正确加载
    Resources::getInstance().Load(path, path);
    info.img = Resources::getInstance().getImage(path);

    // 2. 设置参数
    info.row = row;
    info.col = col;
    info.flag = 1;
    totalFrames = frames;
    interval = speed;
    info.index = 0;
}

void Animation::update() {
    timer += DELTA_TIME;
    if (timer >= interval) {
        timer = 0;
        info.index++;
        if (info.index >= totalFrames) {
            if (loop) {
                info.index = 0;
            } else {
                info.index = totalFrames - 1;
            }
        }
    }
}

const SpriteInfo& Animation::getSpriteInfo() const { return info; }

void Animator::addAnimation(std::string name, std::string path, int row,
                            int col, int frames, double speed) {
    Animation anim;
    anim.Load(path, row, col, frames, speed);
    animations[name] = anim;

    // 如果是第一个动画，设置为当前动画
    if (curAnim == nullptr) {
        curAnim = &animations[name];
    }
}

void Animator::play(std::string name) {
    if (animations.find(name) != animations.end()) {
        curAnim = &animations[name];
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