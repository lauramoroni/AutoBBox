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

Box::Box()
{
	const char* filename = "Resources/flecha.png";

	sprite = new Sprite(filename);

	AutoBBox autoBBox = AutoBBox(filename);
	autoBBox.generate_poly_bbox();

	std::vector<Point> vertices = loadVerticesFromFile(filename);

	BBox(new Poly(vertices.data(), vertices.size()));

	MoveTo(226, 180);
	type = BOX;
}

Box::~Box()
{
	delete sprite;
}

// ---------------------------------------------------------------------------------

Wheel::Wheel()
{
	const char* filename = "Resources/cogumelo.png";

	sprite = new Sprite(filename);

	AutoBBox autoBBox = AutoBBox(filename);
	autoBBox.generate_poly_bbox();

	std::vector<Point> vertex = loadVerticesFromFile(filename);

	BBox(new Poly(vertex.data(), vertex.size()));
	MoveTo(426, 180);
	type = WHEEL;
}

Wheel::~Wheel()
{
	delete sprite;
}

// ---------------------------------------------------------------------------------

Drop::Drop()
{
	const char* filename = "Resources/The-Witcher-3.png";

	sprite = new Sprite(filename);
	
	AutoBBox autoBBox = AutoBBox(filename);
	autoBBox.generate_poly_bbox();
	
	std::vector<Point> vertex = loadVerticesFromFile(filename);

	BBox(new Poly(vertex.data(), vertex.size()));

	MoveTo(576, 180);
	type = DROP;
}

Drop::~Drop()
{
	delete sprite;
}

// ---------------------------------------------------------------------------------

Plane::Plane()
{
	const char* filename = "Resources/mcqueen_rodao.png";

	sprite = new Sprite(filename);

	AutoBBox autoBBox = AutoBBox(filename);
	autoBBox.generate_poly_bbox();

	std::vector<Point> vertex = loadVerticesFromFile(filename);

	BBox(new Poly(vertex.data(), vertex.size()));

	MoveTo(726, 180);
	type = PLANE;
}

Plane::~Plane()
{
	delete sprite;
}

// ---------------------------------------------------------------------------------

Hammer::Hammer()
{
	const char* filename = "Resources/pokemon.png";

	sprite = new Sprite(filename);

	AutoBBox autoBBox = AutoBBox(filename);
	autoBBox.generate_poly_bbox();
	std::vector<Point> vertex = loadVerticesFromFile(filename);

	BBox(new Poly(vertex.data(), vertex.size()));

	MoveTo(926, 180);
	type = HAMMER;
}

Hammer::~Hammer()
{
	delete sprite;
}

// ---------------------------------------------------------------------------------

Display::Display()
{
	sprite = new Sprite("Resources/Display.png");
	//BBox(new Rect(-47, -49, 46, 49));
	MoveTo(226, 380);
	type = DISPLAY;
}

Display::~Display()
{
	delete sprite;
}

// ---------------------------------------------------------------------------------

Ball::Ball()
{
	sprite = new Sprite("Resources/Ball.png");
	//BBox(new Circle(49));
	MoveTo(426, 380);
	type = BALL;
}

Ball::~Ball()
{
	delete sprite;
}

// ---------------------------------------------------------------------------------

Dot::Dot()
{
	sprite = new Sprite("Resources/Dot.png");
	//BBox(new Point());
	MoveTo(576, 380);
	type = DOT;
}

Dot::~Dot()
{
	delete sprite;
}

// ---------------------------------------------------------------------------------

Bolt::Bolt()
{
	sprite = new Sprite("Resources/Bolt.png");

	//MoveTo(726, 380);
	type = BOLT;
}

Bolt::~Bolt()
{
	delete sprite;
}

// ---------------------------------------------------------------------------------

House::House()
{
	sprite = new Sprite("Resources/House.png");
	Point vertex[3] = { Point(-50,-4), Point(0,-42), Point(49,-4) };

	Mixed* mixed = new Mixed();
	mixed->Insert(new Rect(-42, -4, 41, 40));
	mixed->Insert(new Rect(20, -40, 32, -4));
	mixed->Insert(new Poly(vertex, 3));

	//BBox(mixed);
	MoveTo(926, 380);
	type = HOUSE;
}

House::~House()
{
	delete sprite;
}

// ---------------------------------------------------------------------------------