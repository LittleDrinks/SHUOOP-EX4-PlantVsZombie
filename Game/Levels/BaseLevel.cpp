#include "BaseLevel.h"

void BaseLevel::spawnZombie(int type, int row) {
    const double y = row * 100 + 45;
    const double spawnX = WIN_WIDTH + 30;
    if (type == 0) {
        auto* z = GameStatic::createObject<NormalZombie>(Point(spawnX, y));
        if (z) z->row = row;
    }
}

void BaseLevel::spawnSkySun() {
    const int x = RandInt(80, WIN_WIDTH - 80);
    const int landing = RandInt(150, WIN_HEIGHT - 80);
    Sun* sun = GameStatic::createObject<Sun>(Point((double)x, -40.0));
    if (sun) {
        sun->setLandingY((double)landing);
    }
}

void BaseLevel::update() {
    elapsed += DELTA_TIME;
    skySunTimer += DELTA_TIME;

    // 定时掉落阳光
    if (skySunTimer >= skySunInterval) {
        skySunTimer = 0.0;
        spawnSkySun();
    }

    // 按时间表刷僵尸
    while (nextSpawn < zombiePlan.size() && elapsed >= zombiePlan[nextSpawn].time) {
        spawnZombie(zombiePlan[nextSpawn].type, zombiePlan[nextSpawn].row);
        ++nextSpawn;
    }
}
