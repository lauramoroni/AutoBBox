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
#include <string>
using std::stringstream;

// ------------------------------------------------------------------------------

class AutoBBox; // Forward declaration

// ------------------------------------------------------------------------------

enum Objs { MOUSE, CUSTOM };

// ------------------------------------------------------------------------------

class CollisionT : public Game
{
private:
    Font * font = nullptr;          // exibi��o de texto normal
    Font * bold = nullptr;          // exibi��o de texto negrito

    Object * currentObj = nullptr;
    std::string currentFilename;    // nome do arquivo atual carregado
    AutoBBox* currentBBox = nullptr; // bounding box atual para preview e save

    bool viewBBox = true;           // visualiza��o da bounding box (let's default to true maybe or keep false)
    stringstream text;              // convers�o para texto

public:
    static Scene * scene;           // cena do jogo
    static bool mouseClicked;       // estado global do clique para a frame atual

    void Init();                    // inicializa��o
    void Update();                  // atualiza��o
    void Draw();                    // desenho
    void Finalize();                // finaliza��o
};

// ---------------------------------------------------------------------------------

#endif
