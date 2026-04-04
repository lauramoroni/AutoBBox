#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

class StbImage
{
private:
	int width;
	int height;
	int channels;
	int max_val_grayscale; // Valor máximo para imagens em escala de cinza (0-255) para formato PGM
	size_t size;
	uint8_t* pixel_data;

	const char* filename;
public:
	StbImage(const char* filename);
	~StbImage();
	StbImage operator+(const StbImage& other); // adição de imagens (soma pixel a pixel)
	StbImage operator-(const StbImage& other); // subtração de imagens (diferença pixel a pixel)
	StbImage operator*(const StbImage& other); // multiplicação de imagens (multiplicação pixel a pixel)
	StbImage operator/(const StbImage& other); // divisão de imagens (divisão pixel a pixel)
	StbImage operator&(const StbImage& other); // AND lógico
	StbImage operator|(const StbImage& other); // OR lógico
	StbImage operator^(const StbImage& other); // XOR lógico
};