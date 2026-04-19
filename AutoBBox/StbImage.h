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
	StbImage(int width, int height, int channels, uint8_t* pixel_data);
	~StbImage();

	StbImage sum(const StbImage& other, int method = MEAN); // adição de imagens (soma pixel a pixel)
	StbImage subtract(const StbImage& other, int method = MEAN); // subtração de imagens (diferença pixel a pixel)
	StbImage multiply(const StbImage& other, int method = MEAN); // multiplicação de imagens (multiplicação pixel a pixel)
	StbImage divide(const StbImage& other, int method = MEAN); // divisão de imagens (divisão pixel a pixel)
	StbImage logicalAnd(const StbImage& other, int method = MEAN); // AND lógico
	StbImage logicalOr(const StbImage& other, int method = MEAN); // OR lógico
	StbImage logicalXor(const StbImage& other, int method = MEAN); // XOR lógico

	// Transformações Geométricas
	StbImage translate(int dx, int dy);
	StbImage rotate(float angle_degrees);
	StbImage scale(float sx, float sy); // Opcional se for usar o Zoom, mas atende a escala geral
	StbImage reflect(bool horizontal, bool vertical);
	StbImage shear(float shx, float shy);

	// Transformações Compostas
	StbImage applyCompositeTransform(float matrix[3][3]);

	// Zoom IN
	StbImage zoomInReplication(float factor);
	StbImage zoomInInterpolation(float factor);

	// Zoom OUT
	StbImage zoomOutExclusion(int factor);
	StbImage zoomOutMean(int factor);
private:
	void normalize(uint16_t* pixel_data, uint8_t* normalized_pixel_data, int image_elements_size, int method = MEAN); // Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o método especificado (média ou truncamento)
};