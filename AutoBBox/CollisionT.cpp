/**********************************************************************************
// AutoBBox (Cdigo Fonte)
//
// Criao:     26 Jul 2019
// Atualizao: 28 Set 2023
// Compilador:  Visual C++ 2022
//
// Descrio:   Teste de Coliso
//
**********************************************************************************/

#include "Engine.h"
#include "CollisionT.h"
#include "Shapes.h"
#include "Mouse.h"
#include "AutoBBox.h"
#include <windows.h>
#include <commdlg.h>
#include <string>

// ------------------------------------------------------------------------------

Scene * CollisionT::scene = nullptr;            // cena do jogo

// ------------------------------------------------------------------------------

std::string openFileDialog() {
    char filename[MAX_PATH];
    OPENFILENAMEA ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "PNG Files\0*.png\0All Files\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Selecione uma Imagem";
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    
    if (GetOpenFileNameA(&ofn)) {
        return std::string(filename);
    }
    return "";
}

void CollisionT::Init()
{
    // cria fontes para exibio de texto
    font = new Font("Resources/Tahoma14.png");
    font->Spacing("Resources/Tahoma14.dat");
    bold = new Font("Resources/Tahoma14b.png");
    bold->Spacing("Resources/Tahoma14b.dat");

    // cria cena do jogo
    scene = new Scene();

    // adiciona mouse na cena
    scene->Add(new Mouse(), MOVING);
}

// ------------------------------------------------------------------------------

void CollisionT::Update()
{
    // sai com o pressionamento da tecla ESC
    if (window->KeyDown(VK_ESCAPE))
        window->Close();

    // habilita/desabilita bounding box
    if (window->KeyPress('B'))
        viewBBox = !viewBBox;

    // upload button simulation (canto superior direito)
    if (window->KeyPress(VK_LBUTTON)) {
        float mx = window->MouseX();
        float my = window->MouseY();
        if (mx > window->Width() - 100 && my < 40) {
            std::string path = openFileDialog();
            if (!path.empty()) {
                if (currentObj) {
                    scene->Delete(currentObj, MOVING);
                }
                currentObj = new CustomShape(path.c_str());
                scene->Add(currentObj, MOVING);
            }
        }
    }

    // deslocamento padro
    float delta = 100 * gameTime;

    if (currentObj) {
        // desloca objeto selecionado
        if (window->KeyDown(VK_RIGHT))
            currentObj->Translate(delta, 0);
        if (window->KeyDown(VK_LEFT))
            currentObj->Translate(-delta, 0);
        if (window->KeyDown(VK_UP))
            currentObj->Translate(0, -delta);
        if (window->KeyDown(VK_DOWN))
            currentObj->Translate(0, delta);

        // altera escala e rotao do objeto
        if (window->KeyDown('S'))
            currentObj->Scale(1 + 0.005f * delta);
        if (window->KeyDown('A'))
            currentObj->Scale(1 - 0.005f * delta);
        if (window->KeyDown('Z'))
            currentObj->Rotate(-0.5f * delta);
        if (window->KeyDown('X'))
            currentObj->Rotate(0.5f * delta);

        // restaura objeto para seu estado inicial
        if (window->KeyPress('R'))
        {
            currentObj->MoveTo(window->CenterX(), window->CenterY());
            currentObj->RotateTo(0);
            currentObj->ScaleTo(1);
        }
    }

    // atualiza cena do jogo
    scene->Update();
    scene->CollisionDetection();
}

// ------------------------------------------------------------------------------

void CollisionT::Draw()
{
    // desenha cena
    scene->Draw();

    // define cor dos textos
    Color textColor{ 0.65f, 0.65f, 0.65f, 1.0f };
    Color buttonColor{ 0.85f, 0.85f, 0.85f, 1.0f };

    // Boto de upload no canto superior direito
    font->Draw(window->Width() - 90, 20.0f, "[ Upload ]", buttonColor);

    // desenha bounding box dos objetos
    if (viewBBox)
        scene->DrawBBox();
}

// ------------------------------------------------------------------------------

void CollisionT::Finalize()
{
    delete scene;
    delete font;
    delete bold;
}

// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, 
                     _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    Engine* engine = new Engine();

    // configura motor
    engine->window->Mode(WINDOWED);
    engine->window->Size(1152, 648);
    engine->window->Color(20, 20, 20);
    engine->window->Title("Teste de Colisao - Upload");
    engine->window->Icon(IDI_ICON);
    engine->window->Cursor(IDC_CURSOR);

    // inicia o jogo
    int status = engine->Start(new CollisionT());

    delete engine;
    return status;
}

// ----------------------------------------------------------------------------


