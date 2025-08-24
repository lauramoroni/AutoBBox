#ifndef _AUTOBBOX_H_
#define _AUTOBBOX_H_


#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


struct stb_Image {
	int width;
	int height;
	int channels;
	size_t size;
	uint8_t* pixel_data;
};


class AutoBBox
{
private:
	const char* filename = nullptr;						// Nome do arquivo da imagem
	stb_Image image;									// Estrutura que contém os dados da imagem

public:
	AutoBBox(const char* filename);						// Construtor que carrega a imagem
	~AutoBBox();										// Destrutor que libera a memória
	void generate_binarized_image();					// Gera a imagem binarizada
};

#endif