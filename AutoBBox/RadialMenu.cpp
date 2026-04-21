#include "RadialMenu.h"
#include "CollisionT.h"
#include "Shapes.h"
#include "StbImage.h"
#include "AutoBBox.h"
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

RadialMenu::RadialMenu()
{
	bg = new Sprite("Resources/radial_menu_bg.png");
	sep = new Sprite("Resources/radial_menu_sep.png");

	font = new Font("Resources/Tahoma14.png");
	font->Spacing("Resources/Tahoma14.dat");

	// "Por hora, o menu deve ter 4 opcoes da classe StbImage"
	options.push_back("Sum");
	options.push_back("Subtract");
	options.push_back("Multiply");
	options.push_back("Divide");

	active = false;
}

RadialMenu::~RadialMenu()
{
    delete bg;
    delete sep;
    delete font;
}

void RadialMenu::Update()
{
	// Toggle active state with control key
	if (window->KeyDown(VK_CONTROL)) {
		// Get selected shapes
		std::vector<CustomShape*> selectedShapes;
		CollisionT::scene->Begin();
		Object* obj = nullptr;
		while ((obj = CollisionT::scene->Next()) != nullptr) {
			if (CustomShape* cs = dynamic_cast<CustomShape*>(obj)) {
				if (cs->selected) {
					selectedShapes.push_back(cs);
				}
			}
		}

		// Validação: menu só abre ou funciona se houverem exatamente 2 itens selecionados
		if (selectedShapes.size() != 2) {
			active = false;
			return;
		}

		if (!active) {
			MoveTo(window->MouseX(), window->MouseY());
			OutputDebugStringA(("Radial menu activated at (" + std::to_string(window->MouseX()) + ", " + std::to_string(window->MouseY()) + ")\n").c_str());
			active = true;
		}

		if (active && CollisionT::mouseClicked) {
			float dx = window->MouseX() - X();
			float dy = window->MouseY() - Y();
			float dist = sqrt(dx*dx + dy*dy);

			OutputDebugStringA(("Mouse click at (" + std::to_string(window->MouseX()) + ", " + std::to_string(window->MouseY()) + "), Distance: " + std::to_string(dist) + "\n").c_str());

			if (dist >= 15.0f && dist <= 160.0f) {
				int n = options.size();
				if (n > 0) {
					float angle = atan2(dy, dx) * 180.0f / M_PI;
					float angleStep = 360.0f / n;

					float adjustedAngle = angle + 90.0f + (angleStep / 2.0f);
					while(adjustedAngle < 0) adjustedAngle += 360.0f;
					while(adjustedAngle >= 360.0f) adjustedAngle -= 360.0f;

					int clickedIndex = (int)(adjustedAngle / angleStep);
					// Write on debug console
					OutputDebugStringA(("Clicked angle: " + std::to_string(angle) + ", Adjusted angle: " + std::to_string(adjustedAngle) + ", Clicked index: " + std::to_string(clickedIndex) + "\n").c_str());

					std::string op = options[clickedIndex];

					std::string f1 = selectedShapes[0]->imgFilename;
					std::string f2 = selectedShapes[1]->imgFilename;

					StbImage img1(f1.c_str());
					StbImage img2(f2.c_str());

					std::string outFilename;

					if (op == "Sum") {
						img1.sum(img2, MEAN);
						outFilename = "Resources/sum.png";
					}
					else if (op == "Subtract") {
						img1.subtract(img2, MEAN);
						outFilename = "Resources/sub.png";
					}
					else if (op == "Multiply") {
						img1.multiply(img2, MEAN);
						outFilename = "Resources/mul.png";
					}
					else if (op == "Divide") {
						img1.divide(img2, MEAN);
						outFilename = "Resources/div.png";
					}

					if (!outFilename.empty()) {
						AutoBBox* bbox = new AutoBBox(outFilename.c_str());
						bbox->GeneratePolyBBox();

						if (bbox->GetVertices() && bbox->GetVertexCount() > 0) {
							CustomShape* newShape = new CustomShape(outFilename.c_str(), bbox->GetVertices(), bbox->GetVertexCount());
							newShape->selected = true;

							selectedShapes[0]->selected = false;
							selectedShapes[1]->selected = false;

							CollisionT::scene->Add(newShape, MOVING);
						}
						delete bbox;

						active = false; // Fecha o menu com sucesso
					}
				}
			}
		}
	}
	else {
		active = false;
	}
}

void RadialMenu::Draw()
{
    if (!active) return;

    // Draw background centered on Mouse
    bg->Draw(X(), Y(), Layer::UPPER);

    int n = options.size();
    if (n == 0) return;

    float angleStep = 360.0f / n;
    float innerRadius = 40.0f;
    float outerRadius = 125.0f;
    float sepCenterRadius = innerRadius + (85.0f / 2.0f);

    for (int i = 0; i < n; i++) {
        // Separadores nas diagonais das opções (para deixar o item no meio da fatia)
        float itemAngle = i * angleStep - 90.0f; // Começar no topo (-90 graus)
        float sepAngle = itemAngle - (angleStep / 2.0f);
        float sepRad = sepAngle * (float)M_PI / 180.0f;

        // O separador original parece ser vertical. Para apontar para fora do centro,
        // adicionamos 90 graus à rotação de desenho.
        float sepDrawAngle = sepAngle + 90.0f;

        float sepX = X() + sepCenterRadius * cos(sepRad);
        float sepY = Y() + sepCenterRadius * sin(sepRad);

        sep->Draw(sepX, sepY, Layer::FRONT, 1.0f, sepDrawAngle);

        // Posicionar texto exatamente no ângulo do item
        float textRad = itemAngle * (float)M_PI / 180.0f;

        float textRadius = sepCenterRadius;

        // Arredondar as coordenadas evita que o texto fique borrado (sub-pixel rendering)
        float tx = std::round(X() + textRadius * cos(textRad));
        float ty = std::round(Y() + textRadius * sin(textRad));

        font->Draw(tx, ty, options[i], { 1.0f, 1.0f, 1.0f, 1.0f }, Layer::FRONT);
    }
}
