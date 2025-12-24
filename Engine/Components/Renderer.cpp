#include "Renderer.h"
#include "../GameStatic.h"
#include "../Resources.h"

void SpriteRenderer::update() {
    VisualComponent::update();
    if (bFecthAni && getOwner() && getOwner()->getAniSource().flag) {
        sprite = getOwner()->getAniSource();
    }
}

void SpriteRenderer::load(std::string path) {
    Resources::getInstance().Load(path, path);
    sprite.img = Resources::getInstance().getImage(path);
}

void SpriteRenderer::setFetchAni(bool fetch) { bFecthAni = fetch; }

void SpriteRenderer::Render() {
    if (sprite.img) {
        Point pos = getWorldPosition();

        // 计算帧动画的切割区域
        // 如果 row/col 没设置，默认为 1，即显示整张图
        int rows = (sprite.row > 0) ? sprite.row : 1;
        int cols = (sprite.col > 0) ? sprite.col : 1;

        int w = sprite.img->getwidth() / cols;
        int h = sprite.img->getheight() / rows;

        // 计算当前帧在原图中的坐标 (srcX, srcY)
        int currentFrame = sprite.index;
        int srcX = (currentFrame % cols) * w;
        int srcY = (currentFrame / cols) * h;

        // 获取目标设备上下文（屏幕）
        HDC dstDC = GetImageHDC(NULL);

        // 获取源设备上下文（图片）
        HDC srcDC = GetImageHDC(sprite.img);

        // 混合参数：使用 Alpha 通道
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

        // 执行混合绘制
        // 注意：这里源矩形的参数 (srcX, srcY, w, h) 实现了裁剪
        AlphaBlend(dstDC, (int)pos.x, (int)pos.y, w, h, srcDC, srcX, srcY, w, h, bf);
    }
}

StaticMesh::StaticMesh() {
    renderer = GameStatic::createComponent<SpriteRenderer>(Point(0, 0));
    renderer->setAttachment(root);
}

VisualComponent* StaticMesh::getRenderer() const { return renderer; }