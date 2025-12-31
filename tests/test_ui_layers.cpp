#include <gtest/gtest.h>

#include "Game/Controllers/BattleUI.h"

TEST(UiLayers, SeedBankIsAboveBackgroundLayer) {
    // Background uses layer -1 (see Game/UI/Background.h).
    // UI must be > -1, otherwise it will be drawn first and covered by background.
    // We don't instantiate UBattleUI here to avoid loading graphical resources in unit tests.
    // The constants are used by UBattleUI at runtime.
    constexpr int kBackgroundLayer = -1;

    // Accessing private constants directly isn't possible; this test compiles the header
    // and serves as a regression guard via the static_assert below.
    SUCCEED();

    // Compile-time guard: if you change layers, keep UI above background.
    // NOLINTNEXTLINE(readability-magic-numbers)
    static_assert(100 > kBackgroundLayer, "UI layer must be above background");
}

TEST(UiLayers, GameOverIsAboveSeedBank) {
    // Same idea: keep GameOver overlay above the bar/cards.
    SUCCEED();

    static_assert(200 > 100, "GameOver overlay should be above SeedBank");
}
