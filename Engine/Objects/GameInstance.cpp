#include "GameInstance.h"

#include <algorithm>

GameInstance* GlobalInstance = nullptr;

GameInstance::GameInstance() {
	GlobalInstance = this;
}

void GameInstance::setSun(int value) {
	sunShine = std::max(0, value);
}

void GameInstance::addSun(int value) {
	sunShine += value;
	if (sunShine < 0) sunShine = 0;
}

bool GameInstance::spendSun(int value) {
	if (value <= 0) return true;
	if (sunShine < value) return false;
	sunShine -= value;
	return true;
}