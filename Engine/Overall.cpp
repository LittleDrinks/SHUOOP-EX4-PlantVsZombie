#include "Overall.h"
#include "Resources.h"

const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;
const double DELTA_TIME = 0.01;

std::set<DWORD*> gameLastTimes;
DWORD pauseTime = 0;
IMAGE pauseImage;
bool pause = false;

Resources& ResourcePool = Resources::getInstance();
