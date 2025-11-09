#ifndef _AUTOBBOX_H_
#define _AUTOBBOX_H_


#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "Geometry.h"


struct stb_Image {
	int width;
	int height;
	int channels;
	size_t size;
	uint8_t* pixel_data;
};

enum { LEFT_UP, LEFT, LEFT_DOWN, DOWN, RIGHT_DOWN, RIGHT, RIGHT_UP, UP };

class AutoBBox
{
private:
	const char* filename = nullptr;						// Nome do arquivo da imagem
	stb_Image image = { 0, 0, 0, 0, nullptr };			// Estrutura que contém os dados da imagem

public:
	AutoBBox(const char* filename);						// Construtor que carrega a imagem
	~AutoBBox();										// Destrutor que libera a memória

	void generate_poly_bbox();							// Gera a imagem binarizada
};

#endif