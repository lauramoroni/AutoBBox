/**********************************************************************************
// Movable (Arquivo de Cabe�alho)
//
// Cria��o:     27 Jul 2019
// Atualiza��o: 28 Set 2023
// Compilador:  Visual C++ 2022
//
// Descri��o:   Objeto mov�vel atrav�s do mouse
//
**********************************************************************************/

#ifndef _AutoBBox_MOVABLE_H_
#define _AutoBBox_MOVABLE_H_

// ---------------------------------------------------------------------------------

#include "Types.h"                              // tipos espec�ficos da engine
#include "Object.h"                             // objetos do jogo
#include "Sprite.h"                             // desenho de sprites

// ---------------------------------------------------------------------------------

class Movable : public Object
{
protected:
    Sprite * sprite;                            // sprite do objeto
    bool colliding;                             // objeto em colis�o
    bool following;                             // modo de persegui��o do mouse
    float dx, dy;                               // dist�ncia para a posi��o do mouse

public:
    Movable();                                  // construtor
    virtual ~Movable();                         // destrutor

    virtual void OnCollision(Object* obj);      // resolu��o da colis�o
    virtual void Update();                      // atualiza��o
    virtual void Draw();                        // desenho
};

// ---------------------------------------------------------------------------------

#endif
