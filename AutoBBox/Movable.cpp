/**********************************************************************************
// Movable (C�digo Fonte)
//
// Cria��o:     27 Jul 2019
// Atualiza��o: 28 Set 2023
// Compilador:  Visual C++ 2022
//
// Descri��o:   Objeto mov�vel atrav�s do mouse
//
**********************************************************************************/

#include "CollisionT.h"
#include "Movable.h"

// ---------------------------------------------------------------------------------

Movable::Movable()
{
    sprite = nullptr;
    colliding = false;
    following = false;
    selected = false;
    dx = dy = 0;
}

// ---------------------------------------------------------------------------------

Movable::~Movable()
{

}

// ---------------------------------------------------------------------------------

void Movable::OnCollision(Object * obj)
{
    if (obj->Type() != MOUSE)
    {
        // colis�o com qualquer outro objeto
        colliding = true;
    }
    else
    {
        // colisão com o cursor do mouse
        // Não reage se o menu radial (E) estiver sendo ativado
        if (CollisionT::mouseClicked && !window->KeyDown('E'))
        {
            following = true;
            dx = x - window->MouseX();
            dy = y - window->MouseY();

            if (window->KeyDown(VK_SHIFT)) {
                selected = !selected;
            } else {
                selected = true;
            }
        }
    }
}

// ---------------------------------------------------------------------------------

void Movable::Update()
{
    colliding = false;

    if (following)
    {
        MoveTo(window->MouseX() + dx, window->MouseY() + dy);

        if (!window->KeyDown(VK_LBUTTON)) {
            following = false;
        }
    }
}

// ---------------------------------------------------------------------------------

void Movable::Draw()
{
    Color c = Color(1, 1, 1, 1);
    if (colliding)
        c = Color(1, 0, 0, 1); // vermelho
    else if (selected)
        c = Color(0.5f, 1.0f, 0.5f, 1.0f); // verdinho pra indicar selecionado

    sprite->Draw(x, y, z, scale, rotation, c);
}

// ---------------------------------------------------------------------------------