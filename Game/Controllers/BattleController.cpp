#include "BattleController.h"

#include "BattleUI.h"
#include "../Entities/Plants/PeaShooter.h"
#include "../Entities/Plants/SunFlower.h"
#include "../../Engine/coreMinimal.h"

BasePlant* plants[9][5]{};

ABattleController::ABattleController() {
    SetPossession();
    ui = GameStatics::CreateUI<UBattleUI>();
    ui->ShowOnScreen();
}

BasePlant* ABattleController::spawnPlantByIndex(int index, Point pos) {
    if (index == 0) return GameStatics::CreateObject<PeaShooter>(pos);
    if (index == 1) return GameStatics::CreateObject<SunFlower>(pos);
    return nullptr;
}

bool ABattleController::canUseCard(int index) const {
    if (index < 0 || index >= kCardCount) return false;
    if (cardRemaining[index] > 0.0) return false;
    return GameStatics::GetInstance()->getSun() >= cardCosts[index];
}

void ABattleController::startCooldown(int index) {
    if (index < 0 || index >= kCardCount) return;
    cardRemaining[index] = cardCooldown[index];
}

void ABattleController::tickCooldowns() {
    for (int i = 0; i < kCardCount; ++i) {
        if (cardRemaining[i] > 0.0) {
            cardRemaining[i] -= DELTA_TIME;
            if (cardRemaining[i] < 0.0) cardRemaining[i] = 0.0;
        }
    }
}

void ABattleController::refreshCardVisuals() {
    if (!ui) return;
    for (int i = 0; i < kCardCount; ++i) {
        if (!ui->Seeds[i]) continue;
        int alpha = 255;
        if (cardRemaining[i] > 0.0) {
            alpha = 100;
        } else if (GameStatics::GetInstance()->getSun() < cardCosts[i]) {
            alpha = 180;
        }
        ui->Seeds[i]->SetTrans(alpha);
    }
}

void ABattleController::cleanDeadPlants() {
    for (int c = 0; c < kCols; ++c) {
        for (int r = 0; r < kRows; ++r) {
            if (plants[c][r] && GameObjects.find(plants[c][r]) == GameObjects.end()) {
                plants[c][r] = nullptr;
            }
        }
    }
}

void ABattleController::update() {
    if (!ui) return;

    // 关键：引擎主循环里 UI::Update() 在 Object::update() 之后才调用，
    // 这会导致 ui->index（鼠标悬停卡片）滞后一帧，从而出现“点卡片/点草坪没反应”。
    // 这里先手动刷新一次，保证本帧输入使用的是最新 hover 结果。
    ui->Update();

    cleanDeadPlants();
    tickCooldowns();
    refreshCardVisuals();

    // 检查游戏失败
    for (int c = 0; c < kCols; ++c) {
        for (int r = 0; r < kRows; ++r) {
            // 简单判定：如果有僵尸进入左侧区域（x < 0），则游戏失败
            // 这里需要遍历所有僵尸，暂时无法直接访问僵尸列表，
            // 但可以通过 BaseLevel 或全局对象管理。
            // 暂时留空，等待后续完善僵尸管理。
        }
    }

    // 临时：通过全局状态或消息判断游戏失败
    // 假设 BaseZombie 会在到达左边界时设置某个标志，或者这里直接遍历所有对象（效率较低但可行）
    // 为了简单起见，我们在 BaseZombie 中添加了逻辑，如果到达左边界，可以触发一个全局事件。
    // 由于没有全局事件系统，我们这里简单遍历一下 GameObjects 找僵尸。
    for (auto* obj : GameObjects) {
        if (auto* z = dynamic_cast<BaseZombie*>(obj)) {
            if (z->getWorldPosition().x < -50) {
                ui->ShowGameOver();
                // 可以在这里暂停游戏
                // GameStatics::GetInstance()->Pause();
                break;
            }
        }
    }

    const ExMessage& cursor = GetCursorMessage();
    const bool pressed = cursor.lbutton && !prevLbutton;
    const bool released = !cursor.lbutton && prevLbutton;
    prevLbutton = cursor.lbutton;

    const Point coor = FindCoordinate(GetCursorWorldPosition());
    const int hoverCol = (int)coor.x;
    const int hoverRow = (int)coor.y;
    const bool hoverInField = (hoverCol >= 0 && hoverCol < kCols && hoverRow >= 0 && hoverRow < kRows);

    // 交互语义：
    // 1) 点卡片：拿起植物（花费与冷却在“种下时”结算/开始）
    // 2) 点草坪：尝试在格子中种下（支持“按下”或“松开”放置）
    if (pressed) {
        // 点卡片：切换当前选中植物
        if (ui->index >= 0 && ui->index < kCardCount) {
            if (plant) {
                plant->Destroy();
                plant = nullptr;
            }
            if (prePlant) {
                prePlant->Destroy();
                prePlant = nullptr;
            }

            if (canUseCard(ui->index)) {
                curIndex = ui->index;
                plant = spawnPlantByIndex(curIndex, GetCursorWorldPosition());
                if (plant) {
                    if (auto* r = dynamic_cast<VisualComponent*>(plant->getRenderer())) {
                        r->setLayer(10);
                    }
                }
            } else {
                curIndex = -1;
            }
        }

        // 点草坪：尝试种下
        // 新需求：如果点击位置不合法/种植失败，则取消当前选择（销毁手里拿着的植物）。
        if (plant && ui->index < 0) {
            bool placed = false;
            if (hoverInField) {
                if (!plants[hoverCol][hoverRow] && canUseCard(curIndex) && GameStatics::GetInstance()->spendSun(cardCosts[curIndex])) {
                    plant->setPosition(Point(hoverCol * 80 + 30, hoverRow * 100 + 90));
                    if (auto* r = dynamic_cast<VisualComponent*>(plant->getRenderer())) {
                        r->setLayer(hoverRow + 2);
                    }
                    plant->setEnabled(true);
                    plants[hoverCol][hoverRow] = plant;
                    startCooldown(curIndex);
                    plant = nullptr;
                    curIndex = -1;
                    placed = true;
                }
            }

            if (!placed) {
                plant->Destroy();
                plant = nullptr;
                curIndex = -1;
            }
        }
    }

    // 支持“拖拽松开种下”（更符合直觉，也兼容你之前的操作方式）
    if (released) {
        if (plant && ui->index < 0) {
            bool placed = false;
            if (hoverInField) {
                if (!plants[hoverCol][hoverRow] && canUseCard(curIndex) && GameStatics::GetInstance()->spendSun(cardCosts[curIndex])) {
                    plant->setPosition(Point(hoverCol * 80 + 30, hoverRow * 100 + 90));
                    if (auto* r = dynamic_cast<VisualComponent*>(plant->getRenderer())) {
                        r->setLayer(hoverRow + 2);
                    }
                    plant->setEnabled(true);
                    plants[hoverCol][hoverRow] = plant;
                    startCooldown(curIndex);
                    plant = nullptr;
                    curIndex = -1;
                    placed = true;
                }
            }

            if (!placed) {
                plant->Destroy();
                plant = nullptr;
                curIndex = -1;
            }
        }
    }

    // 没拿着植物时，清理预览
    if (!plant) {
        if (prePlant) {
            prePlant->Destroy();
            prePlant = nullptr;
        }
    }

    // 放置预览：仅当当前选中卡片且格子为空
    if (!plant && ui->index < 0 && curIndex >= 0) {
        if (hoverInField && !plants[hoverCol][hoverRow] && canUseCard(curIndex)) {
            if (!prePlant) {
                prePlant = spawnPlantByIndex(curIndex, GetCursorWorldPosition());
                if (prePlant) {
                    if (auto* r = dynamic_cast<VisualComponent*>(prePlant->getRenderer())) {
                        // 提升预览层级，确保预览在植物栏/卡片之上，但仍低于某些临时渲染层
                        r->setLayer(5);
                        r->setTransparency(100);
                    }
                }
            }
        } else {
            if (prePlant) prePlant->Destroy();
            prePlant = nullptr;
        }
    }

    // 拖拽跟随
    if (plant) {
        plant->setPosition(GetCursorWorldPosition() - Point(40, 40));
    }
    if (prePlant) {
        if (hoverInField) {
            prePlant->setPosition(Point(hoverCol * 80 + 30, hoverRow * 100 + 90));
        }
    }
}
