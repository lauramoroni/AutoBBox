/**********************************************************************************
// AutoBBox (Codigo Fonte)
//
// Criacao:     26 Jul 2019
// Atualizacao: 28 Set 2023
// Compilador:  Visual C++ 2022
//
// Descricao:   Teste de Colisao
//
**********************************************************************************/

#include "Engine.h"
#include "CollisionT.h"
#include "Shapes.h"
#include "Mouse.h"
#include "AutoBBox.h"
#include "RadialMenu.h"
#include "StbImage.h" // Adicionado para suportar o StbImage nas opções
#include <windows.h>
#include <commdlg.h>
#include <string>
#include <memory>
#include <algorithm>

// ------------------------------------------------------------------------------

Scene* CollisionT::scene = nullptr;            // cena do jogo
bool CollisionT::mouseClicked = false;          // clique do mouse

// ------------------------------------------------------------------------------

std::string openFileDialog() {
    char filename[MAX_PATH];
    OPENFILENAMEA ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;

    // Filtro atualizado para suportar PNG, JPG, JPEG e PGM
    ofn.lpstrFilter = "Image Files\0*.png;*.jpg;*.jpeg;*.pgm\0All Files\0*.*\0";

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

std::vector<CustomShape*> CollisionT::GetSelectedShapes() {
    std::vector<CustomShape*> selected;
    if (!scene) return selected;
    scene->Begin();
    Object* obj = nullptr;
    while ((obj = scene->Next()) != nullptr) {
        if (CustomShape* cs = dynamic_cast<CustomShape*>(obj)) {
            if (cs->selected) {
                selected.push_back(cs);
            }
        }
    }
    return selected;
}

void CollisionT::Init()
{
    // cria fontes para exibicao de texto
    font = new Font("Resources/Tahoma14.png");
    font->Spacing("Resources/Tahoma14.dat");
    bold = new Font("Resources/Tahoma14b.png");
    bold->Spacing("Resources/Tahoma14b.dat");

    // carregar backgrounds
    background = new Sprite("Resources/background.png");
    sideMenu = new Sprite("Resources/side_menu_bg.png");
    btnUpload = new Sprite("Resources/select_image_button.png");

    // cria cena do jogo
    scene = new Scene();

    // adiciona mouse na cena
    scene->Add(new Mouse(), MOVING);

    auto applyOperation = [this](const std::string& opName) {
        auto selectedShapes = this->GetSelectedShapes();
        if (selectedShapes.size() != 2) return;

        std::string f1 = selectedShapes[0]->imgFilename;
        std::string f2 = selectedShapes[1]->imgFilename;

        StbImage img1(f1.c_str());
        StbImage img2(f2.c_str());

        // Carrega as propriedades de rotação e escala da cena
        float scale1 = selectedShapes[0]->Scale();
        float rot1 = selectedShapes[0]->Rotation();

        float scale2 = selectedShapes[1]->Scale();
        float rot2 = selectedShapes[1]->Rotation();

        // Aplica transformações fisicas da Engine no buffer de dados da Imagem
        if (scale1 != 1.0f) img1 = img1.scale(scale1, scale1);
        if (rot1 != 0.0f) img1 = img1.rotate(rot1);

        if (scale2 != 1.0f) img2 = img2.scale(scale2, scale2);
        if (rot2 != 0.0f) img2 = img2.rotate(rot2);

        std::string outFilename;
        static int tempCounter = 0;
        outFilename = "Resources/~temp_op_" + std::to_string(++tempCounter) + ".png";

        StbImage result(0, 0, 0, nullptr);

        if (opName == "Sum") result = img1.sum(img2, MEAN);
        else if (opName == "Subtract") result = img1.subtract(img2, MEAN);
        else if (opName == "Multiply") result = img1.multiply(img2, MEAN);
        else if (opName == "Divide") result = img1.divide(img2, MEAN);
        else if (opName == "AND") result = img1.logicalAnd(img2, MEAN);
        else if (opName == "OR") result = img1.logicalOr(img2, MEAN);
        else if (opName == "XOR") result = img1.logicalXor(img2, MEAN);

        if (result.pixel_data) {
            result.save(outFilename.c_str());
            AutoBBox* bbox = new AutoBBox(outFilename.c_str());
            bbox->GeneratePolyBBox();

            if (bbox->GetVertices() && bbox->GetVertexCount() > 0) {
                CustomShape* newShape = new CustomShape(outFilename.c_str(), bbox->GetVertices(), bbox->GetVertexCount());
                newShape->selected = true;

                // Move para o centro da área visível (excluindo menu lateral de 144px)
                newShape->MoveTo((window->Width() - 144) / 2.0f, window->CenterY());

                selectedShapes[0]->selected = false;
                selectedShapes[1]->selected = false;

                CollisionT::scene->Add(newShape, MOVING);
            }
            delete bbox;
        }
        };

    // Lambda para lidar com transformações de espaço de cor (Cores e Recomposição)
    auto applySpaceTransform = [this](const std::string& opName) {
        auto selectedShapes = this->GetSelectedShapes();
        if (selectedShapes.size() != 1) return;

        std::string filename = selectedShapes[0]->imgFilename;
        StbImage img(filename.c_str());

        // Carrega propriedades físicas
        float scale = selectedShapes[0]->Scale();
        float rot = selectedShapes[0]->Rotation();

        if (scale != 1.0f) img = img.scale(scale, scale);
        if (rot != 0.0f) img = img.rotate(rot);

        std::vector<StbImage> results;
        if (opName == "CMY") results = img.toCMY();
        else if (opName == "CMYK") results = img.toCMYK();
        else if (opName == "HSB") results = img.toHSB();
        else if (opName == "HSL") results = img.toHSL();
        else if (opName == "YUV") results = img.toYUV();
        else if (opName == "ExtractChannels") {
            results.push_back(img.extractChannelR_color());
            results.push_back(img.extractChannelG_color());
            results.push_back(img.extractChannelB_color());
        }

        selectedShapes[0]->selected = false;

        static int transCounter = 0;
        transCounter++;

        for (size_t i = 0; i < results.size(); ++i) {
            std::string outFilename = "Resources/~temp_" + opName + "_" + std::to_string(transCounter) + "_" + std::to_string(i) + ".png";

            if (results[i].pixel_data) {
                results[i].save(outFilename.c_str());

                AutoBBox* bbox = new AutoBBox(outFilename.c_str());
                bbox->GeneratePolyBBox();

                if (bbox->GetVertices() && bbox->GetVertexCount() > 0) {
                    CustomShape* newShape = new CustomShape(outFilename.c_str(), bbox->GetVertices(), bbox->GetVertexCount());
                    newShape->selected = true;

                    // Distribuir itens diagonalmente para não ficarem escondidos
                    float offsetX = (i * 80.0f) - ((results.size() - 1) * 40.0f);
                    float offsetY = (i * 30.0f);

                    newShape->MoveTo((window->Width() - 144) / 2.0f + offsetX, window->CenterY() + offsetY);

                    CollisionT::scene->Add(newShape, MOVING);
                }
                delete bbox;
            }
        }
        };

    RadialMenuOptions rootMenu;
    RadialMenuOptions algMenu;
    RadialMenuOptions logMenu;
    RadialMenuOptions singleMenu;
    RadialMenuOptions colorsMenu;
    RadialMenuOptions recomposeMenu;

    // Menu para um item selecionado
    singleMenu.AddOption("Export BBox", [this]() {
        auto selectedShapes = this->GetSelectedShapes();
        if (selectedShapes.size() != 1) return;
        std::string filename = selectedShapes[0]->imgFilename;
        std::string base_name = filename;
        size_t last_slash = base_name.find_last_of("/\\");
        if (std::string::npos != last_slash) {
            base_name.erase(0, last_slash + 1);
        }
        size_t last_dot = base_name.find_last_of(".");
        if (std::string::npos != last_dot) {
            base_name.erase(last_dot);
        }
        base_name += "_vertices.txt";

        std::string savePath = saveFileDialog(base_name);
        if (!savePath.empty()) {
            AutoBBox bbox(filename.c_str());
            bbox.GeneratePolyBBox();
            bbox.WriteVerticesToFile(savePath.c_str());
        }
        });

    singleMenu.AddOption("Export Image", [this]() {
        auto selectedShapes = this->GetSelectedShapes();
        if (selectedShapes.size() != 1) return;
        std::string filename = selectedShapes[0]->imgFilename;
        std::string base_name = filename;
        size_t last_slash = base_name.find_last_of("/\\");
        if (std::string::npos != last_slash) {
            base_name.erase(0, last_slash + 1);
        }
        size_t last_dot = base_name.find_last_of(".");
        if (std::string::npos != last_dot) {
            base_name.erase(last_dot);
        }
        base_name += "_export.png";

        char path[MAX_PATH];
        ZeroMemory(&path, sizeof(path));
        strncpy_s(path, base_name.c_str(), MAX_PATH - 1);

        OPENFILENAMEA ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFilter = "PNG Files\0*.png\0All Files\0*.*\0";
        ofn.lpstrFile = path;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrTitle = "Salvar Imagem";
        ofn.lpstrDefExt = "png";
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        if (GetSaveFileNameA(&ofn)) {
            StbImage img(filename.c_str());
            float scale = selectedShapes[0]->Scale();
            float rot = selectedShapes[0]->Rotation();
            if (scale != 1.0f) img = img.scale(scale, scale);
            if (rot != 0.0f) img = img.rotate(rot);
            img.save(path);
        }
        });

    singleMenu.AddOption("Colors", [this]() {
        this->activeMenu = this->colorsMenuObj;
        this->activeMenu->MoveTo(window->MouseX(), window->MouseY());
        });

    // Opções do Root Menu (Itens Duplos)
    rootMenu.AddOption("Algebricas", [this]() {
        this->activeMenu = this->algMenuObj;
        this->activeMenu->MoveTo(window->MouseX(), window->MouseY());
        });
    rootMenu.AddOption("Binarias", [this]() {
        this->activeMenu = this->logMenuObj;
        this->activeMenu->MoveTo(window->MouseX(), window->MouseY());
        });

    // Opções de Álgebra
    algMenu.AddOption("Sum", [applyOperation]() { applyOperation("Sum"); });
    algMenu.AddOption("Subtract", [applyOperation]() { applyOperation("Subtract"); });
    algMenu.AddOption("Multiply", [applyOperation]() { applyOperation("Multiply"); });
    algMenu.AddOption("Divide", [applyOperation]() { applyOperation("Divide"); });

    // Opções Lógicas
    logMenu.AddOption("AND", [applyOperation]() { applyOperation("AND"); });
    logMenu.AddOption("OR", [applyOperation]() { applyOperation("OR"); });
    logMenu.AddOption("XOR", [applyOperation]() { applyOperation("XOR"); });

    // Opções de Cores
    colorsMenu.AddOption("Extract RGB", [applySpaceTransform]() { applySpaceTransform("ExtractChannels"); });
    colorsMenu.AddOption("Decompose", [this]() {
        this->activeMenu = this->recomposeMenuObj;
        this->activeMenu->MoveTo(window->MouseX(), window->MouseY());
        });

    // Opções de Recomposição
    recomposeMenu.AddOption("CMY", [applySpaceTransform]() { applySpaceTransform("CMY"); });
    recomposeMenu.AddOption("CMYK", [applySpaceTransform]() { applySpaceTransform("CMYK"); });
    recomposeMenu.AddOption("HSB", [applySpaceTransform]() { applySpaceTransform("HSB"); });
    recomposeMenu.AddOption("HSL", [applySpaceTransform]() { applySpaceTransform("HSL"); });
    recomposeMenu.AddOption("YUV", [applySpaceTransform]() { applySpaceTransform("YUV"); });

    // Inicialização dos Objetos do Menu
    rootMenuObj = new RadialMenu(rootMenu);
    algMenuObj = new RadialMenu(algMenu);
    logMenuObj = new RadialMenu(logMenu);
    singleMenuObj = new RadialMenu(singleMenu);
    colorsMenuObj = new RadialMenu(colorsMenu);
    recomposeMenuObj = new RadialMenu(recomposeMenu);
}

// ------------------------------------------------------------------------------

void CollisionT::Update()
{
    // desseleciona figuras com a tecla ESC e fecha menu se aberto
    if (window->KeyPress(VK_ESCAPE)) {
        if (activeMenu) {
            activeMenu->Deactivate();
            activeMenu = nullptr;
            rootMenuObj->Deactivate();
            algMenuObj->Deactivate();
            logMenuObj->Deactivate();
            singleMenuObj->Deactivate();
            colorsMenuObj->Deactivate();
            recomposeMenuObj->Deactivate();
        }
        else {
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
    }

    // habilita/desabilita bounding box
    if (window->KeyPress('B'))
        viewBBox = !viewBBox;

    static bool prevMouse = false;
    bool currMouse = window->KeyDown(VK_LBUTTON);
    mouseClicked = currMouse && !prevMouse;
    prevMouse = currMouse;

    static bool prevRMouse = false;
    bool currRMouse = window->KeyDown(VK_RBUTTON);
    bool rMouseClicked = currRMouse && !prevRMouse;
    prevRMouse = currRMouse;

    if (rMouseClicked) {
        size_t selectedCount = GetSelectedShapes().size();
        if (activeMenu) activeMenu->Deactivate();
        rootMenuObj->Deactivate();
        algMenuObj->Deactivate();
        logMenuObj->Deactivate();
        singleMenuObj->Deactivate();
        colorsMenuObj->Deactivate();
        recomposeMenuObj->Deactivate();
        activeMenu = nullptr;

        if (selectedCount == 2) {
            activeMenu = rootMenuObj;
        }
        else if (selectedCount == 1) {
            activeMenu = singleMenuObj;
        }
    }

    if (activeMenu) {
        activeMenu->Update();
        // Se apos o update o "mouseClicked" ainda estiver ativo e foi consumido com clique fora
        // ele vai fechar o menu. Isso garante que nao continue a desenhar o menu.
        if (mouseClicked) {
            activeMenu->Deactivate();
            activeMenu = nullptr;
            rootMenuObj->Deactivate();
            algMenuObj->Deactivate();
            logMenuObj->Deactivate();
            singleMenuObj->Deactivate();
            colorsMenuObj->Deactivate();
            recomposeMenuObj->Deactivate();
            mouseClicked = false;
        }
    }

    // upload button simulation (canto superior direito)
    if (mouseClicked) {
        OutputDebugStringA(("Mouse clicked: " + std::to_string(mouseClicked) + "\n").c_str());
        float mx = window->MouseX();
        float my = window->MouseY();

        float btnX = window->Width() - (144 / 2.0f);
        float btnY = btnUpload ? (58.0f + btnUpload->Height() / 2.0f) : 58.0f;
        float halfW = btnUpload ? (btnUpload->Width() / 2.0f) : 39.0f;
        float halfH = btnUpload ? (btnUpload->Height() / 2.0f) : 13.0f;

        if (mx > (btnX - halfW) && mx < (btnX + halfW) && my >(btnY - halfH) && my < (btnY + halfH)) {
            std::string path = openFileDialog();
            if (!path.empty()) {

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
                    newObj->MoveTo((window->Width() - 144) / 2.0f, window->CenterY());
                    scene->Add(newObj, MOVING);
                }
                currentFilename = path;
            }
        }
    }

    // Desselecionar todos ao clicar fora
    // Não desseleciona se o menu radial estiver aberto (tecla E)
    if (mouseClicked && !window->KeyDown(VK_SHIFT)) {
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

    bool zoomIn = window->KeyPress('I');
    bool zoomOut = window->KeyPress('K');

    if (zoomIn || zoomOut) {
        auto selectedShapes = GetSelectedShapes();

        // Aplica o zoom apenas se houver 1 objeto selecionado
        if (selectedShapes.size() == 1) {
            CustomShape* shape = selectedShapes[0];
            std::string filename = shape->imgFilename;
            StbImage img(filename.c_str());

            StbImage result(0, 0, 0, nullptr);
            bool appliedZoom = false;

            if (zoomIn) {
                // Tecla I: Zoom In (Fator float)
                result = img.zoomInInterpolation(1.5f);
                appliedZoom = true;
            }
            else if (zoomOut) {
                // Tecla K: Zoom Out (Fator int)
                result = img.zoomOutMean(2);
                appliedZoom = true;
            }

            if (appliedZoom && result.pixel_data) {
                static int zoomCounter = 0;
                std::string outFilename = "Resources/~temp_zoom_" + std::to_string(++zoomCounter) + ".png";

                // Salva a nova imagem processada
                result.save(outFilename.c_str());

                // Recalcula o Bounding Box para a nova imagem
                AutoBBox* bbox = new AutoBBox(outFilename.c_str());
                bbox->GeneratePolyBBox();

                if (bbox->GetVertices() && bbox->GetVertexCount() > 0) {
                    CustomShape* newShape = new CustomShape(outFilename.c_str(), bbox->GetVertices(), bbox->GetVertexCount());
                    newShape->selected = true;

                    // Mantém a posição e rotação do objeto original
                    newShape->MoveTo(shape->X(), shape->Y());
                    newShape->RotateTo(shape->Rotation());

                    // Substitui a imagem na cena
                    scene->Delete(shape, MOVING);
                    scene->Add(newShape, MOVING);
                }
                delete bbox;
            }
        }
    }

    bool reflectH = window->KeyPress('H');
    bool reflectV = window->KeyPress('V');
    bool shearXPos = window->KeyPress('M');
    bool shearXNeg = window->KeyPress('N');
    bool shearYPos = window->KeyPress('J');
    bool shearYNeg = window->KeyPress('U');

    if (reflectH || reflectV || shearXPos || shearXNeg || shearYPos || shearYNeg) {
        auto selectedShapes = GetSelectedShapes();

        // Aplica a transformação apenas se houver 1 objeto selecionado
        if (selectedShapes.size() == 1) {
            CustomShape* shape = selectedShapes[0];
            std::string filename = shape->imgFilename;
            StbImage img(filename.c_str());

            StbImage result(0, 0, 0, nullptr);
            bool appliedTransform = false;

            // Define o incremento do cisalhamento
            float shearFactor = 0.15f;

            if (reflectH) {
                result = img.reflect(true, false);
                appliedTransform = true;
            }
            else if (reflectV) {
                result = img.reflect(false, true);
                appliedTransform = true;
            }
            else if (shearXPos) {
                result = img.shear(shearFactor, 0.0f);
                appliedTransform = true;
            }
            else if (shearXNeg) {
                result = img.shear(-shearFactor, 0.0f);
                appliedTransform = true;
            }
            else if (shearYPos) {
                result = img.shear(0.0f, shearFactor);
                appliedTransform = true;
            }
            else if (shearYNeg) {
                result = img.shear(0.0f, -shearFactor);
                appliedTransform = true;
            }

            if (appliedTransform && result.pixel_data) {
                static int transformCounter = 0;
                std::string outFilename = "Resources/~temp_transform_" + std::to_string(++transformCounter) + ".png";

                // Salva a nova imagem processada
                result.save(outFilename.c_str());

                // Recalcula o Bounding Box para a nova imagem
                AutoBBox* bbox = new AutoBBox(outFilename.c_str());
                bbox->GeneratePolyBBox();

                if (bbox->GetVertices() && bbox->GetVertexCount() > 0) {
                    CustomShape* newShape = new CustomShape(outFilename.c_str(), bbox->GetVertices(), bbox->GetVertexCount());
                    newShape->selected = true;

                    // Mantém a posição e rotação do objeto original na cena
                    newShape->MoveTo(shape->X(), shape->Y());
                    newShape->RotateTo(shape->Rotation());

                    // Substitui a imagem antiga na cena pela nova transformada
                    scene->Delete(shape, MOVING);
                    scene->Add(newShape, MOVING);
                }
                delete bbox;
            }
        }
    }

    // deslocamento padrão
    float delta = 100 * gameTime;

    scene->Begin();
    Object* objIter = nullptr;
    while ((objIter = scene->Next()) != nullptr) {
        Movable* mov = dynamic_cast<Movable*>(objIter);
        if (mov && mov->selected) {
            // desloca objeto selecionado
            if (window->KeyDown(VK_RIGHT)) mov->Translate(delta, 0);
            if (window->KeyDown(VK_LEFT)) mov->Translate(-delta, 0);
            if (window->KeyDown(VK_UP)) mov->Translate(0, -delta);
            if (window->KeyDown(VK_DOWN)) mov->Translate(0, delta);

            // altera escala e rotação do objeto
            if (window->KeyDown('S')) mov->Scale(1 + 0.005f * delta);
            if (window->KeyDown('A')) mov->Scale(1 - 0.005f * delta);
            if (window->KeyDown('Z')) mov->Rotate(-0.5f * delta);
            if (window->KeyDown('X')) mov->Rotate(0.5f * delta);

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
    if (background)
        background->Draw(window->CenterX(), window->CenterY(), Layer::BACK);
    if (sideMenu)
        sideMenu->Draw(window->Width() - sideMenu->Width() / 2.0f, window->CenterY(), Layer::LOWER);

    // desenha cena
    scene->Draw();

    Color textColor{ 0.65f, 0.65f, 0.65f, 1.0f };
    Color buttonColor{ 0.85f, 0.85f, 0.85f, 1.0f };

    // Upload
    if (btnUpload) {
        float btnX = window->Width() - (144 / 2.0f);
        float btnY = 58.0f + (btnUpload->Height() / 2.0f);
        btnUpload->Draw(btnX, btnY, Layer::FRONT);
    }

    // desenha bounding box dos objetos
    if (viewBBox)
        scene->DrawBBox();

    if (activeMenu) {
        activeMenu->Draw();
    }
}

// ------------------------------------------------------------------------------

void CollisionT::Finalize()
{
    if (currentBBox) {
        delete currentBBox;
        currentBBox = nullptr;
    }

    if (background) delete background;
    if (sideMenu) delete sideMenu;
    if (btnUpload) delete btnUpload;

    delete scene;
    delete font;
    delete bold;

    if (rootMenuObj) delete rootMenuObj;
    if (algMenuObj) delete algMenuObj;
    if (logMenuObj) delete logMenuObj;
    if (singleMenuObj) delete singleMenuObj;
    if (colorsMenuObj) delete colorsMenuObj;
    if (recomposeMenuObj) delete recomposeMenuObj;
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