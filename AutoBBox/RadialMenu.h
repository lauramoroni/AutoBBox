#ifndef _RADIAL_MENU_H_
#define _RADIAL_MENU_H_

#include "Object.h"
#include "Sprite.h"
#include "Font.h"
#include <vector>
#include <string>

class RadialMenu : public Object
{
private:
    Sprite* bg;
    Sprite* sep;
    Font* font;
    std::vector<std::string> options;
    bool active;

public:
    RadialMenu();
    ~RadialMenu();

    void Update();
    void Draw();
};

#endif
