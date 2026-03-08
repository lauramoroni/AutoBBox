/**********************************************************************************
// Shapes (C�digo Fonte)
//
// Cria��o:     27 Jul 2019
// Atualiza��o: 28 Set 2023
// Compilador:  Visual C++ 2022
//
// Descri��o:   Objetos de diferentes formas
//
**********************************************************************************/

#include "CollisionT.h"
#include "Shapes.h"
#include "AutoBBox.h"
#include <vector>
#include <fstream> 
#include <string>
#include <sstream> 
#include <iostream>

static std::vector<Point> loadVerticesFromFile(const char* filename) {
	std::vector<Point> vertices;
	
	std::string base_name;
	if (filename) {
		base_name = filename;
		size_t last_slash = base_name.find_last_of("/\\");
		if (std::string::npos != last_slash) {
			base_name.erase(0, last_slash + 1);
		}
		size_t last_dot = base_name.find_last_of(".");
		if (std::string::npos != last_dot) {
			base_name.erase(last_dot);
		}
	}

	std::string vertices_path = std::string("Resources/bbox/") + base_name + "_vertices.txt";

	std::ifstream inputFile(vertices_path);

	// Verifica se o arquivo foi aberto com sucesso
	if (!inputFile.is_open()) {
		std::cerr << "Erro: Nao foi possivel abrir o arquivo de coordenadas: " << vertices_path << std::endl;
	}

	else {
		std::string line;
		int x, y;

		while (std::getline(inputFile, line))
		{
			std::stringstream ss(line);
			if (ss >> x >> y)
			{
				// 5. Adiciona o novo ponto ao vetor
				vertices.push_back(Point(x, y));
			}
		}

		inputFile.close();
	}

	return vertices;
}

// ---------------------------------------------------------------------------------

CustomShape::CustomShape(const char* filename)
{
	sprite = new Sprite(filename);

	AutoBBox autoBBox = AutoBBox(filename);
	autoBBox.generate_poly_bbox();

	std::vector<Point> vertices = loadVerticesFromFile(filename);
    if (!vertices.empty()) {
	    BBox(new Poly(vertices.data(), vertices.size()));
    }

	MoveTo(576, 324); // Center of a ~1152x648 window roughly
	type = CUSTOM;
}

CustomShape::~CustomShape()
{
	if (sprite) delete sprite;
}

// ---------------------------------------------------------------------------------