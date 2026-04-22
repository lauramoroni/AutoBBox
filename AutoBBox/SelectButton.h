#ifndef _SELECT_BUTTON_H_
#define _SELECT_BUTTON_H_

#include "Object.h"
#include "TileSet.h"
#include "Animation.h"
#include "CollisionT.h"

// Matches the StbImage enum: MEAN = 0, TRUNCATE = 1
enum NormalizeMethod { MEAN = 0, TRUNCATE = 1 };

class SelectButton : public Object
{
private:
    TileSet* tileSet;
    Animation* anim;
    NormalizeMethod currentMethod;

public:
    SelectButton();
    ~SelectButton();

    void Update() override;
    void Draw()   override;

    NormalizeMethod GetMethod() const { return currentMethod; }
};

#endif