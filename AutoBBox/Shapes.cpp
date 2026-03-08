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

// ---------------------------------------------------------------------------------

CustomShape::CustomShape(const char* filename, Point* vertices, int vertexCount)
{
	sprite = new Sprite(filename);

	if (vertices && vertexCount > 0) {
		BBox(new Poly(vertices, vertexCount));
	}

	MoveTo(576, 324); // Center of a ~1152x648 window roughly
	type = CUSTOM;
}

CustomShape::~CustomShape()
{
	if (sprite) delete sprite;
}

// ---------------------------------------------------------------------------------