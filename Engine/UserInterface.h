#pragma once

#include <set>
#include <string>

#include "Pvz2CompatGlobals.h"
#include "coreMinimal.h"
#include "GameStatic.h"
#include "Components/Renderer.h"
#include "Resources.h"

// PVZ2-like UI system (compat layer)

class Widget {
protected:
    Point location{};
    Widget* parent = nullptr;
    std::set<Widget*> sons;

    Point size{0, 0};
    bool visible = false;

public:
    Widget() { GamePainters.insert(this); }
    virtual ~Widget() {
        GamePainters.erase(this);
        for (auto* s : sons) {
            delete s;
        }
        sons.clear();
    }

    virtual void Update() {}
    virtual void Paint() {}

    void SetAttachment(Widget* par) {
        if (parent) parent->sons.erase(this);
        parent = par;
        if (parent) parent->sons.insert(this);
    }

    void SetSize(Point si) { size = si; }
    Point GetSize() const { return size; }

    virtual void SetVisibility(bool v) { visible = v; }
    bool GetVisibility() const { return visible; }

    void SetLocation(Point loc) { location = loc; }
    Point GetRelativeLocation() const { return location; }

    Point GetAbsoluteLocation() const {
        if (parent) return location + parent->GetAbsoluteLocation();
        return location;
    }

protected:
    bool IsUnderCursor() const {
        if (!MainController) return false;
        const Point p = MainController->GetCursorScreenPosition();
        const Point loc = GetAbsoluteLocation();
        return p.x > loc.x && p.x < (loc.x + size.x) && p.y > loc.y && p.y < (loc.y + size.y);
    }
};

class UIRenderer : public VisualComponent {
public:
    UIRenderer() = default;
    ~UIRenderer() override = default;

    void Load(const std::string& path, int wid, int hei) {
        // 复用工程内 Resources，避免 UNICODE/MBCS 下 loadimage 重载不匹配
        Resources::getInstance().Load(path, path, wid, hei);
        sprite.img = Resources::getInstance().getImage(path);
        sprite.name = path;
        setFixEnable(false);
    }

    void SetImage(IMAGE* img) { sprite.img = img; }

    void Render() override {
        if (!sprite.img) return;

        const Point pos = getWorldPosition() + sprite.delta;
        const int w = sprite.img->getwidth();
        const int h = sprite.img->getheight();

        HDC dstDC = GetImageHDC(NULL);
        HDC srcDC = GetImageHDC(sprite.img);

        BLENDFUNCTION bf = { AC_SRC_OVER, 0, getTransparency(), AC_SRC_ALPHA };

        if (sprite_.isUsed) {
            AlphaBlend(dstDC, (int)pos.x, (int)pos.y, (int)sprite_.aimSize.x, (int)sprite_.aimSize.y,
                       srcDC, (int)sprite_.aimLoc.x, (int)sprite_.aimLoc.y, (int)sprite_.aimSize.x, (int)sprite_.aimSize.y, bf);
        } else {
            AlphaBlend(dstDC, (int)pos.x, (int)pos.y, w, h, srcDC, 0, 0, w, h, bf);
        }
    }
};

class Image : public Widget {
protected:
    UIRenderer* ima = nullptr;
    IMAGE* ui = nullptr;
    int layer = 0;

public:
    Image() {
        ima = GameStatic::createComponent<UIRenderer>(Point(0, 0));
        ui = new IMAGE();
        ima->SetImage(ui);
        GameRenders.erase(ima);
    }

    ~Image() override {
        if (ima) {
            GameRenders.erase(ima);
            ima->Destruct();
        }
        delete ui;
    }

    void Update() override {
        if (ima) ima->setPosition(GetAbsoluteLocation());
    }

    void SetVisibility(bool v) override {
        visible = v;
        if (!ima) return;
        if (v) {
            GameRenders.insert(ima);
            ima->setPosition(GetAbsoluteLocation());
            ima->setLayer(layer);
        } else {
            GameRenders.erase(ima);
        }
    }

    void LoadPicture(const std::string& path, Point si = Point(0, 0)) {
        const Point temp = (si.x == 0 && si.y == 0) ? GetSize() : si;
        if (ima) {
            ima->Load(path, (int)temp.x, (int)temp.y);
            ima->setLayer(layer);
        }
    }

    void SetLayer(int lay) {
        layer = lay;
        if (ima) ima->setLayer(layer);
    }

    void SetTrans(int tran) {
        if (ima) ima->setTransparency((BYTE)tran);
    }

    bool IsCursorOn() { return visible && IsUnderCursor(); }
};

class Text : public Widget {
    std::string text;
public:
    void SetText(const std::string& t) { text = t; }

    void Paint() override {
        if (!visible) return;
    #if defined(UNICODE) || defined(_UNICODE)
        std::wstring w(text.begin(), text.end());
        outtextxy((int)GetAbsoluteLocation().x, (int)GetAbsoluteLocation().y, w.c_str());
#else
        outtextxy((int)GetAbsoluteLocation().x, (int)GetAbsoluteLocation().y, text.c_str());
#endif
    }
};

class UserInterface {
protected:
    std::set<Widget*> widgets;

public:
    virtual ~UserInterface() {
        for (auto* w : widgets) delete w;
        widgets.clear();
    }

    virtual void Update() {
        for (auto* w : widgets) {
            if (w->GetVisibility()) w->Update();
        }
    }

    template <class T>
    T* AddWidget(Point pos) {
        T* gameWid = new T;
        gameWid->SetLocation(pos);
        widgets.insert(gameWid);
        return gameWid;
    }

    void ShowOnScreen() {
        for (auto* w : widgets) w->SetVisibility(true);
    }

    void HideFromScreen() {
        for (auto* w : widgets) w->SetVisibility(false);
    }

    void RemoveFromScreen() {
        for (auto* w : widgets) delete w;
        widgets.clear();
        GameUIs_.push_back(this);
    }
};
