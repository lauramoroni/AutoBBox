/**********************************************************************************
// CollisionT (Arquivo de Cabe�alho)
//
// Cria��o:     26 Jul 2019
// Atualiza��o: 28 Set 2023
// Compilador:  Visual C++ 2022
//
// Descri��o:   Teste de Colis�o
//
**********************************************************************************/

#ifndef _CollisionT_H_
#define _CollisionT_H_

// ------------------------------------------------------------------------------

#include "Game.h"
#include "Sprite.h"
#include "TileSet.h"
#include "Animation.h"
#include "Scene.h"
#include "Font.h"
#include "Resources.h"
#include <sstream>
using std::stringstream;

// ------------------------------------------------------------------------------

enum Objs { BOX, WHEEL, DROP, PLANE, HAMMER, DISPLAY, BALL, DOT, BOLT, HOUSE, MOUSE };

// ------------------------------------------------------------------------------

class CollisionT : public Game
{
private:
    Sprite * backg = nullptr;       // pano de fundo
    Sprite * overlay = nullptr;     // painel sobreposto

    TileSet * selSet = nullptr;     // folha de sprites da caixa de sele��o
    Animation * select = nullptr;   // anima��o da caixa de sele��o
    
    Font * font = nullptr;          // exibi��o de texto normal
    Font * bold = nullptr;          // exibi��o de texto negrito

    struct {
        float x;                    // coordenada x
        float y;                    // coordenada y
    } pos[10];                      // posi��o inicial dos objetos

    Object * obj[10] = {0};         // vetor de objetos

    bool viewBBox = false;          // visualiza��o da bounding box
    stringstream text;              // convers�o para texto    

public:
    static int index;               // �ndice do vetor
    static Scene * scene;           // cena do jogo

    void Init();                    // inicializa��o
    void Update();                  // atualiza��o
    void Draw();                    // desenho
    void Finalize();                // finaliza��o
};

// ---------------------------------------------------------------------------------

#endif
