#pragma once

#include <set>

#include "Pvz2CompatGlobals.h"
#include "coreMinimal.h"
#include "GameStatic.h"
#include "Objects/GameInstance.h"
#include "Objects/UserInterface.h"

// PVZ2-like GameStatics (compat layer)
class GameStatics {
public:
    template <class T>
    static T* CreateObject(Point pos) {
        return GameStatic::createObject<T>(pos);
    }

    template <class T>
    static std::set<T*> GetAllObjects() {
        std::set<T*> out;
        for (auto* obj : GameObjects) {
            if (auto* casted = dynamic_cast<T*>(obj)) out.insert(casted);
        }
        return out;
    }

    static void DeleteObjects() {
        for (auto* obj : GameObjects) {
            delete obj;
        }
        GameObjects.clear();
    }

    template <class T>
    static T* ConstructComponent(Point pos) {
        return GameStatic::createComponent<T>(pos);
    }

    template <class T>
    static T* CreateUI() {
        T* ui = new T;
        GameUIs.insert(static_cast<UserInterface*>(ui));
        return ui;
    }

    static Controller* GetController() {
        return MainController ? MainController : GameStatic::getPlayerController();
    }

    static GameInstance* GetInstance() {
        static GameInstance instance;
        return &instance;
    }
};
