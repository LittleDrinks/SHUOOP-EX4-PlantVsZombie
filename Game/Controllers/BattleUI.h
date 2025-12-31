#pragma once

#include <string>

#include "../../Engine/Objects/UserInterface.h"
#include "../../Engine/GameStatics.h"

// PVZ2-like: minimal UBattleUI (only SeedBank + 1 card)
class UBattleUI : public UserInterface {
    friend class ABattleController;

    static constexpr int kUiLayerSeedBank = 0;
    static constexpr int kUiLayerCardBase = 1;
    static constexpr int kUiLayerGameOver = 200;

    Image* SeedBank = nullptr;
    Image* Seeds[8]{};
    Text* SunText = nullptr;

    // Controller <-> UI
    bool bStart = true;

public:
    int index = -1;
    int failFlag = 0;

    Image* GameOverImage = nullptr;

    UBattleUI() {
        SeedBank = AddWidget<Image>(Point(5, 0));
        SeedBank->SetSize(Point(530, 91));
        SeedBank->LoadPicture("res/images/bar.png");
        // 注意：渲染按 layer 从小到大绘制。背景是 -1；UI 必须 > -1 才不会被盖住。
        SeedBank->SetLayer(kUiLayerSeedBank);
        SeedBank->SetVisibility(true); // 确保可见

        // Only card1 for now, keep array shape like original
        const int wid = 53;
        const int hei = 77;

        Seeds[0] = AddWidget<Image>(Point(80, 6));
        Seeds[0]->SetAttachment(SeedBank);
        Seeds[0]->SetSize(Point(wid, hei));
        Seeds[0]->LoadPicture("res/images/cards/card1.png");
        Seeds[0]->SetLayer(kUiLayerCardBase + 0);
        Seeds[0]->SetVisibility(true); // 确保可见

        Seeds[1] = AddWidget<Image>(Point(80 + (wid + 2), 6));
        Seeds[1]->SetAttachment(SeedBank);
        Seeds[1]->SetSize(Point(wid, hei));
        Seeds[1]->LoadPicture("res/images/cards/card2.png");
        Seeds[1]->SetLayer(kUiLayerCardBase + 1);
        Seeds[1]->SetVisibility(true); // 确保可见

        for (int i = 2; i < 8; ++i) {
            Seeds[i] = AddWidget<Image>(Point(80 + (wid + 2) * i, 6));
            Seeds[i]->SetAttachment(SeedBank);
            Seeds[i]->SetSize(Point(wid, hei));
            Seeds[i]->LoadPicture("res/images/cards/black.png");
            Seeds[i]->SetLayer(kUiLayerCardBase + i);
            Seeds[i]->SetTrans(180);
            Seeds[i]->SetVisibility(true); // 确保可见
        }

        SunText = AddWidget<Text>(Point(22, 65));
        SunText->SetAttachment(SeedBank);
        SunText->SetText("100");
        SunText->SetColor(BLACK);
        SunText->SetVisibility(true); // 确保可见

        GameOverImage = AddWidget<Image>(Point(200, 200));
        GameOverImage->SetSize(Point(400, 200)); // 假设尺寸
        GameOverImage->LoadPicture("res/images/ZombiesWon.png");
        GameOverImage->SetLayer(kUiLayerGameOver);
        GameOverImage->SetVisibility(false);
    }

    void ShowGameOver() {
        if (GameOverImage) {
            GameOverImage->SetVisibility(true);
        }
    }

    void Update() override {
        UserInterface::Update();
        if (!bStart) return;

        // PVZ2-like semantics: hover decides index
        index = -1;
        for (int i = 0; i < 8; ++i) {
            if (Seeds[i] && Seeds[i]->IsCursorOn()) {
                index = i;
                break;
            }
        }

        if (SunText) {
            SunText->SetText(std::to_string(GameStatics::GetInstance()->getSun()));
            SunText->SetVisibility(true);
        }

        // failFlag kept for signature compatibility (not used in this minimal version)
        failFlag = 0;
    }
};
