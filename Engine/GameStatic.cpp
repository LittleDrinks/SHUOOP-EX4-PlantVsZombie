#include "GameStatic.h"

Controller* GameStatic::playerController = nullptr;

Controller* GameStatic::getPlayerController() {
    if (!playerController) {
        playerController = new Controller();
    }
    return playerController;
}