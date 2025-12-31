#include "Sun.h"

#include <algorithm>
#include <cmath>

#include "../../../Engine/Overall.h"
#include "../Base/BaseZombie.h"

Sun::Sun() {
    // 渲染 & 动画
    anim = GameStatic::createComponent<Animator>(Point(0, 0));
    anim->setAttachment(root);
    anim->addAnimation("idle", "res/animations/bullets/sun.png", 2, 6, 12, 0.12);
    anim->play("idle");

    if (auto* r = dynamic_cast<SpriteRenderer*>(renderer)) {
        r->setLayer(10);
    }

    // 碰撞体
    box->setSize(Point(70, 70));
    box->setType("Sun");

    // 生命周期计时
    fadeTimer.bind(7.5, &Sun::beginFade, this, false);
    destroyTimer.bind(10.0, &Sun::removeSelf, this, false);

    landingY = getWorldPosition().y + 120.0;  // 默认向下落一小段
}

void Sun::beginFade() { fading = true; }

void Sun::removeSelf() { Destroy(); }

bool Sun::hitTest(const Point& p) {
    const Point pos = getWorldPosition();
    const Point sz = box->getSize();
    return p.x >= pos.x && p.x <= pos.x + sz.x && p.y >= pos.y && p.y <= pos.y + sz.y;
}

void Sun::update() {
    // 点击收集判定
    if (!collected) {
        if (auto* controller = GameStatics::GetController()) {
            const ExMessage& msg = controller->GetCursorMessage();
            // 修正：使用屏幕坐标进行点击判定，因为鼠标消息是屏幕坐标
            if (msg.lbutton && hitTest(Point((double)msg.x, (double)msg.y))) {
                collected = true;
                fading = false;
            }
        }
    }

    // 已被收集：朝左上角飞去
    if (collected) {
        // 修复：原逻辑用指数衰减趋近 (0,0)，但用 x<0/y<0 判定永远不会触发，
        // 会导致阳光堆在左上角且永不结算。
        const Point pos = getWorldPosition();
        const Point next = StepCollectMove(pos, DELTA_TIME, collectSpeed);
        setPosition(next);

        if (HasReachedCollectTarget(next)) {
            GameStatics::GetInstance()->addSun(25);
            Destroy();
        }
        return;
    }

    // 自然下落到指定 y
    if (falling) {
        addPosition(Point(0.0, fallSpeed));
        if (getWorldPosition().y >= landingY) {
            setPosition(Point(getWorldPosition().x, landingY));
            falling = false;
        }
    }

    fadeTimer.tick();
    destroyTimer.tick();

    if (fading) {
        if (auto* r = dynamic_cast<VisualComponent*>(renderer)) {
            BYTE t = r->getTransparency();
            r->setTransparency(t > 4 ? t - 4 : 0);
            if (r->getTransparency() == 0) {
                Destroy();
            }
        }
    }
}
