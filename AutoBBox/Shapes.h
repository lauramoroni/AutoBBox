/**********************************************************************************
// Shapes (Arquivo de Cabe�alho)
//
// Cria��o:     27 Jul 2019
// Atualiza��o: 28 Set 2023
// Compilador:  Visual C++ 2022
//
// Descri��o:   Objetos de diferentes formas
//
**********************************************************************************/

#ifndef _AutoBBox_SHAPES_H_
#define _AutoBBox_SHAPES_H_

// ---------------------------------------------------------------------------------

#include "Movable.h"
#include <vector>

static std::vector<Point> loadVerticesFromFile(const char* filename);

// ---------------------------------------------------------------------------------

class CustomShape : public Movable
{
public:
    CustomShape(const char* filename);
    ~CustomShape();
};

#endif
