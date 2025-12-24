#include "Resources.h"

Resources& Resources::getInstance() {
    static Resources instance;
    return instance;
}

void Resources::init() {
    // TODO
}

void Resources::Load(std::string name, std::string path, int w, int h) {
    if (imagePool.find(name) != imagePool.end()) return;
    IMAGE* img = new IMAGE();
    loadimage(img, path.c_str(), w, h);
    imagePool[name] = img;
    pathPool[name] = path;
}

void Resources::Load(std::string name, std::string path) {
    Load(name, path, 0, 0);
}

IMAGE* Resources::getImage(std::string name) {
    if (imagePool.find(name) != imagePool.end()) {
        return imagePool[name];
    } else {
        return nullptr;
    }
}

std::string Resources::findPath(std::string name) {
    if (pathPool.find(name) != pathPool.end()) {
        return pathPool[name];
    } else {
        return "";
    }
}