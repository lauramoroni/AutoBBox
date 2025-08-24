/**********************************************************************************
// Mouse (Arquivo de Cabe�alho)
// 
// Cria��o:     27 Jul 2019
// Atualiza��o: 28 Set 2023
// Compilador:  Visual C++ 2022
//
// Descri��o:   Gerencia mouse na tela
//
**********************************************************************************/

#ifndef _AutoBBox_MOUSE_H_
#define _AutoBBox_MOUSE_H_

// ---------------------------------------------------------------------------------

#include "Object.h"        
#include "Types.h"        

// ---------------------------------------------------------------------------------

class Mouse : public Object
{
public:
    Mouse();                            // construtor
    ~Mouse();                           // destrutor

    void OnCollision(Object* obj);      // resolu��o de colis�o
    void Update();                      // atualiza��o
    void Draw();                        // desenho
}; 

// ---------------------------------------------------------------------------------

#endif