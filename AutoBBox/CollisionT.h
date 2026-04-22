/**********************************************************************************
// CollisionT (Arquivo de Cabeçalho)
//
// Criação:     26 Jul 2019
// Atualização: 28 Set 2023
// Compilador:  Visual C++ 2022
//
// Descrição:   Teste de Colisão
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
    Font* font = nullptr;          // exibio de texto normal
    Font* bold = nullptr;          // exibio de texto negrito
    Sprite* background = nullptr;  // background da tela
    Sprite* sideMenu = nullptr;    // background do menu lateral
    Sprite* btnUpload = nullptr;   // botao de upload

    Object* currentObj = nullptr;
    std::string currentFilename;    // nome do arquivo atual carregado
    AutoBBox* currentBBox = nullptr; // bounding box atual para preview e save

    bool viewBBox = true;           // visualização da bounding box 
    stringstream text;              // conversão para texto

public:
    static Scene* scene;           // cena do jogo
    static bool mouseClicked;

    class RadialMenu* rootMenuObj = nullptr;
    class RadialMenu* algMenuObj = nullptr;
    class RadialMenu* logMenuObj = nullptr;
    class RadialMenu* singleMenuObj = nullptr;
    class RadialMenu* colorsMenuObj = nullptr;      // Submenu de Cores
    class RadialMenu* recomposeMenuObj = nullptr;   // Submenu de Recomposição
    class RadialMenu* activeMenu = nullptr;

    std::vector<class CustomShape*> GetSelectedShapes();       // estado global do clique para a frame atual

    void Init();                    // inicialização
    void Update();                  // atualização
    void Draw();                    // desenho
    void Finalize();                // finalização
};

// ---------------------------------------------------------------------------------

#endif