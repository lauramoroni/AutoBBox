#include "SelectButton.h"

// Sprite sheet: 2 frames side by side (1 row x 2 cols)
// Frame 0 = MEAN selected, Frame 1 = TRUNCATE selected
SelectButton::SelectButton()
{
    tileSet = new TileSet("Resources/select_button_sheet.png", 1, 2);
    anim = new Animation(tileSet, 0.0f, false); // delay=0, no loop (manual frame)

    currentMethod = MEAN;
    anim->Frame(0); // start on MEAN frame

    // Center on screen
    MoveTo(window->CenterX(), window->CenterY());
}

SelectButton::~SelectButton()
{
    delete anim;
    delete tileSet;
}

void SelectButton::Update()
{
    if (!CollisionT::mouseClicked) return;

    float mx = window->MouseX();
    float my = window->MouseY();

    // Half-size of one tile frame
    float hw = tileSet->TileWidth() * 0.5f;
    float hh = tileSet->TileHeight() * 0.5f;

    // Hit-test against the full sheet (both frames side by side)
    // Each frame occupies hw pixels wide, centered at X() +/- hw
    float sheetLeft = X() - tileSet->Width() * 0.5f;
    float sheetRight = X() + tileSet->Width() * 0.5f;
    float sheetTop = Y() - hh;
    float sheetBot = Y() + hh;

    if (mx >= sheetLeft && mx <= sheetRight &&
        my >= sheetTop && my <= sheetBot)
    {
        // Determine which half was clicked
        float midX = X(); // center between the two frames
        if (mx < midX)
        {
            // Left frame = MEAN
            currentMethod = MEAN;
            anim->Frame(0);
        }
        else
        {
            // Right frame = TRUNCATE
            currentMethod = TRUNCATE;
            anim->Frame(1);
        }
        CollisionT::mouseClicked = false; // consume click
    }
}

void SelectButton::Draw()
{
    // Draw only the active frame centered on screen
    anim->Draw(
        anim->Frame(),   // explicit frame index
        X(), Y(),
        Layer::UPPER
    );
}