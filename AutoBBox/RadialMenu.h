#ifndef _RADIAL_MENU_H_
#define _RADIAL_MENU_H_

#include "Object.h"
#include "Sprite.h"
#include "Font.h"
#include <vector>
#include <string>
#include <functional>

struct RadialMenuOption {
    std::string name;
    std::function<void()> action;
};

class RadialMenuOptions
{
public:
    std::vector<RadialMenuOption> items;

    void AddOption(const std::string& name, std::function<void()> action) {
        items.push_back({name, action});
    }
};

class RadialMenu : public Object
{
private:
    Sprite* bg;
    Sprite* sep;
    Font* font;
    RadialMenuOptions menuOptions;
    bool active;

public:
    RadialMenu(const RadialMenuOptions& options);
    ~RadialMenu();

    void Update();
    void Draw();
    void Deactivate();
};

#endif
