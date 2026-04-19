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

	StbImage sum(const StbImage& other, int method = MEAN);			// adição de imagens (soma pixel a pixel)
	StbImage subtract(const StbImage& other, int method = MEAN);	// subtração de imagens (diferença pixel a pixel)
	StbImage multiply(const StbImage& other, int method = MEAN);	// multiplicação de imagens (multiplicação pixel a pixel)
	StbImage divide(const StbImage& other, int method = MEAN);		// divisão de imagens (divisão pixel a pixel)
	StbImage logicalAnd(const StbImage& other, int method = MEAN);	// AND lógico
	StbImage logicalOr(const StbImage& other, int method = MEAN);	// OR lógico
	StbImage logicalXor(const StbImage& other, int method = MEAN);	// XOR lógico
	StbImage translate(int dx, int dy);								// Translação da imagem
	StbImage rotate(float angle_degrees);							// Rotação da imagem em torno do centro
	StbImage scale(float sx, float sy);								// Escala da imagem
	StbImage reflect(bool horizontal, bool vertical);				// Reflexão da imagem
	StbImage shear(float shx, float shy);							// Cisalhamento da imagem
	StbImage applyCompositeTransform(float matrix[3][3]);			// Aplicar transformação composta
	StbImage zoomInReplication(float factor);						// Zoom in com replicação
	StbImage zoomInInterpolation(float factor);						// Zoom in com interpolação
	StbImage zoomOutExclusion(int factor);							// Zoom out com exclusão
	StbImage zoomOutMean(int factor);								// Zoom out com média
private:
	void normalize(uint16_t* pixel_data, uint8_t* normalized_pixel_data, int image_elements_size, int method = MEAN); // Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o método especificado (média ou truncamento)
};