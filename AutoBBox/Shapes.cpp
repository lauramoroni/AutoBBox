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
	const char* filename = "Resources/The-Witcher-3.png";

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
	const char* filename = "Resources/pokemon.png";

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
	const char* filename = "Resources/Jigglypuff.png";

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
	sprite = new Sprite("Resources/Plane.png");

	Point vertex[18] =
	{
		Point(-10,-41), Point(-4,-46), Point(4,-46), Point(10,-41),
		Point(10,-22), Point(65,-20), Point(65,-13), Point(7,6),
		Point(2,36), Point(18,41), Point(18,47), Point(-17,47), Point(-17,41), Point(-2,36),
		Point(-7,6), Point(-65,-13), Point(-65,-20), Point(-10,-22)
	};

	//BBox(new Poly(vertex, 18));

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
	sprite = new Sprite("Resources/Hammer.png");

	Mixed* mixed = new Mixed();
	mixed->Insert(new Rect(-28, -48, 26, -23));
	mixed->Insert(new Rect(-6, -50, 5, 48));

	//BBox(mixed);    
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

	Point vertex[88] = {
				Point(-11, -42),
Point(-11, -40),
Point(-12, -39),
Point(-12, -38),
Point(-13, -37),
Point(-13, -36),
Point(-14, -35),
Point(-14, -33),
Point(-15, -32),
Point(-15, -31),
Point(-16, -30),
Point(-16, -29),
Point(-17, -28),
Point(-17, -26),
Point(-18, -25),
Point(-18, -24),
Point(-19, -23),
Point(-19, -22),
Point(-20, -21),
Point(-20, -19),
Point(-21, -18),
Point(-21, -17),
Point(-22, -16),
Point(-22, -15),
Point(-23, -14),
Point(-23, -12),
Point(-24, -11),
Point(-24, -10),
Point(-25, -9),
Point(-25, -8),
Point(-26, -7),
Point(-26, -5),
Point(-27, -4),
Point(-27, -3),
Point(-28, -2),
Point(-28, -1),
Point(-29, 0),
Point(-29, 4),
Point(-19, 4),
Point(-18, 3),
Point(-12, 3),
Point(-11, 4),
Point(-11, 5),
Point(-12, 6),
Point(-12, 8),
Point(-13, 9),
Point(-13, 11),
Point(-14, 12),
Point(-14, 15),
Point(-15, 16),
Point(-15, 18),
Point(-16, 19),
Point(-16, 22),
Point(-17, 23),
Point(-17, 25),
Point(-18, 26),
Point(-18, 29),
Point(-19, 30),
Point(-19, 32),
Point(-20, 33),
Point(-20, 35),
Point(-21, 36),
Point(-21, 39),
Point(-22, 40),
Point(-22, 42),
Point(-23, 43),
Point(-23, 47),
Point(-21, 47),
Point(-17, 43),
Point(-17, 42),
Point(-8, 33),
Point(-8, 32),
Point(0, 24),
Point(0, 23),
Point(8, 15),
Point(8, 14),
Point(16, 6),
Point(16, 5),
Point(24, -3),
Point(24, -4),
Point(29, -9),
Point(29, -10),
Point(28, -11),
Point(7, -11),
Point(6, -12),
Point(34, -40),
Point(34, -41),
Point(33, -42)
	};


	AutoBBox autoBBox = AutoBBox("Resources/Bolt.png");
	autoBBox.generate_poly_bbox();

	BBox(new Poly(vertex, 88));
	MoveTo(726, 380);
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