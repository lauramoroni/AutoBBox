#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

enum { MEAN, TRUNCATE };

class StbImage
{
public:
	int width = 0;
	int height = 0;
	int channels = 0;
	int max_val_grayscale = 0; // Valor máximo para imagens em escala de cinza (0-255) para formato PGM
	size_t size = 0;
	uint8_t* pixel_data = nullptr;

	StbImage(const char* filename);
	~StbImage();

	StbImage operator+(const StbImage& other); // adição de imagens (soma pixel a pixel)
	StbImage operator-(const StbImage& other); // subtração de imagens (diferença pixel a pixel)
	StbImage operator*(const StbImage& other); // multiplicação de imagens (multiplicação pixel a pixel)
	StbImage operator/(const StbImage& other); // divisão de imagens (divisão pixel a pixel)
	StbImage operator&(const StbImage& other); // AND lógico
	StbImage operator|(const StbImage& other); // OR lógico
	StbImage operator^(const StbImage& other); // XOR lógico
private:
	uint8_t normalize(uint16_t* pixel_data, int method = MEAN); // Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o método especificado (média ou truncamento)
};