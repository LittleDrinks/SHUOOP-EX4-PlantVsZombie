#include "BaseBullet.h"

BaseBullet::BaseBullet() {
    box = GameStatic::createComponent<BoxCollider>(Point(0, 0));
    box->setAttachment(root);
    box->setType("Bullet");
}