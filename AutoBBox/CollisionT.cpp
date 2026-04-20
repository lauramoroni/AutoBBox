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
#include "RadialMenu.h"
#include <windows.h>
#include <commdlg.h>
#include <string>

// ------------------------------------------------------------------------------

Scene * CollisionT::scene = nullptr;            // cena do jogo
bool CollisionT::mouseClicked = false;          // clique do mouse

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

std::string saveFileDialog(const std::string& defaultName) {
    char filename[MAX_PATH];
    ZeroMemory(&filename, sizeof(filename));

    // Define nome padrão
    if (!defaultName.empty()) {
        strncpy_s(filename, defaultName.c_str(), MAX_PATH - 1);
    }

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Salvar Vertices";
    ofn.lpstrDefExt = "txt";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileNameA(&ofn)) {
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

    // adiciona radial menu
    scene->Add(new RadialMenu(), MOVING);
}

// ------------------------------------------------------------------------------

void CollisionT::Update()
{
    // desseleciona figuras com a tecla ESC
    if (window->KeyPress(VK_ESCAPE)) {
        scene->Begin();
        Object* obj = nullptr;
        while ((obj = scene->Next()) != nullptr) {
            if (Movable* mov = dynamic_cast<Movable*>(obj)) {
                if (mov->selected) {
                    mov->selected = false;
                }
            }
        }
    }

    // habilita/desabilita bounding box
    if (window->KeyPress('B'))
        viewBBox = !viewBBox;

    static bool prevMouse = false;
    bool currMouse = window->KeyDown(VK_LBUTTON);
	OutputDebugStringA(("Mouse state: " + std::to_string(currMouse) + "\n").c_str());
    mouseClicked = currMouse && !prevMouse;
    prevMouse = currMouse;

    // upload button simulation (canto superior direito)
    if (mouseClicked) {
        OutputDebugStringA(("Mouse clicked: " + std::to_string(mouseClicked) + "\n").c_str());
        float mx = window->MouseX();
        float my = window->MouseY();

        // Botão Upload
        if (mx > window->Width() - 100 && my < 40) {
            std::string path = openFileDialog();
            if (!path.empty()) {
                // Sem remover o objeto anterior

                // Remove bounding box anterior (apenas visualização / último)
                if (currentBBox) {
                    delete currentBBox;
                    currentBBox = nullptr;
                }

                // Gera a bounding box para preview
                currentBBox = new AutoBBox(path.c_str());
                currentBBox->GeneratePolyBBox();

                // Cria o objeto com a bounding box gerada
                Object* newObj = nullptr;
                if (currentBBox->GetVertices() && currentBBox->GetVertexCount() > 0) {
                    newObj = new CustomShape(path.c_str(), currentBBox->GetVertices(), currentBBox->GetVertexCount());

                    // Fazer o novo objeto começar já selecionado
                    if (Movable* mov = dynamic_cast<Movable*>(newObj)) {
                        mov->selected = true;
                    }
                }

                if (newObj) {
                    scene->Add(newObj, MOVING);
                }
                currentFilename = path;
            }
        }
        // Botão Save (aparece após upload)
        else if (!currentFilename.empty() && currentBBox && mx > window->Width() - 100 && my >= 50 && my < 90) {
            // Extrai o nome base do arquivo para sugerir no diálogo
            std::string base_name = currentFilename;
            size_t last_slash = base_name.find_last_of("/\\");
            if (std::string::npos != last_slash) {
                base_name.erase(0, last_slash + 1);
            }
            size_t last_dot = base_name.find_last_of(".");
            if (std::string::npos != last_dot) {
                base_name.erase(last_dot);
            }
            base_name += "_vertices.txt";

            // Abre diálogo para escolher onde salvar
            std::string savePath = saveFileDialog(base_name);
            if (!savePath.empty()) {
                currentBBox->WriteVerticesToFile(savePath.c_str());
            }
        }
    }

    // Desselecionar todos ao clicar fora (sobrescrito se o clique atingir algum, atravs do Movable)
    // Não desseleciona se o menu radial estiver aberto (tecla E)
    if (mouseClicked && !window->KeyDown(VK_SHIFT) && !window->KeyDown('E')) {
        scene->Begin();
        Object* obj = nullptr;
        while ((obj = scene->Next()) != nullptr) {
            if (Movable* mov = dynamic_cast<Movable*>(obj)) {
                mov->selected = false;
            }
        }
    }

    // Deletar selecionados
    if (window->KeyPress(VK_DELETE)) {
        scene->Begin();
        Object* obj = nullptr;
        while ((obj = scene->Next()) != nullptr) {
            if (Movable* mov = dynamic_cast<Movable*>(obj)) {
                if (mov->selected) {
                    scene->Delete(obj, MOVING);
                }
            }
        }
    }

    // deslocamento padro
    float delta = 100 * gameTime;

    scene->Begin();
    Object* objIter = nullptr;
    while ((objIter = scene->Next()) != nullptr) {
        Movable* mov = dynamic_cast<Movable*>(objIter);
        if (mov && mov->selected) {
            // desloca objeto selecionado
            if (window->KeyDown(VK_RIGHT))
                mov->Translate(delta, 0);
            if (window->KeyDown(VK_LEFT))
                mov->Translate(-delta, 0);
            if (window->KeyDown(VK_UP))
                mov->Translate(0, -delta);
            if (window->KeyDown(VK_DOWN))
                mov->Translate(0, delta);

            // altera escala e rotao do objeto
            if (window->KeyDown('S'))
                mov->Scale(1 + 0.005f * delta);
            if (window->KeyDown('A'))
                mov->Scale(1 - 0.005f * delta);
            if (window->KeyDown('Z'))
                mov->Rotate(-0.5f * delta);
            if (window->KeyDown('X'))
                mov->Rotate(0.5f * delta);

            // restaura objeto para seu estado inicial
            if (window->KeyPress('R'))
            {
                mov->MoveTo(window->CenterX(), window->CenterY());
                mov->RotateTo(0);
                mov->ScaleTo(1);
            }
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

    Color textColor{ 0.65f, 0.65f, 0.65f, 1.0f };
    Color buttonColor{ 0.85f, 0.85f, 0.85f, 1.0f };

    // Upload
    font->Draw(window->Width() - 90, 20.0f, "[ Upload ]", buttonColor);

    // Save
    if (!currentFilename.empty() && currentBBox) {
        font->Draw(window->Width() - 90, 60.0f, "[  Save  ]", buttonColor);
    }

    // desenha bounding box dos objetos
    if (viewBBox)
        scene->DrawBBox();
}

// ------------------------------------------------------------------------------

void CollisionT::Finalize()
{
    if (currentBBox) {
        delete currentBBox;
        currentBBox = nullptr;
    }
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


