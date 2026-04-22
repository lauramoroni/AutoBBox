#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <vector>
#include <utility>

enum { MEAN, TRUNCATE };

// Structure to hold a density slicing rule: pixels in [low, high] get color (r,g,b)
struct DensitySlice {
    uint8_t low;
    uint8_t high;
    uint8_t r, g, b;
};

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

	StbImage(const StbImage& other);
	StbImage& operator=(const StbImage& other);
	StbImage(StbImage&& other) noexcept;
	StbImage& operator=(StbImage&& other) noexcept;

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
	bool save(const char* filename);								// Salva a imagem


	StbImage extractChannelR() const;
	StbImage extractChannelG() const;
	StbImage extractChannelB() const;

	StbImage extractChannelR_color() const;
	StbImage extractChannelG_color() const;
	StbImage extractChannelB_color() const;

	static StbImage recompose(const StbImage& rPlane, const StbImage& gPlane, const StbImage& bPlane);

	std::vector<StbImage> toCMY() const;
	std::vector<StbImage> toCMYK() const;
	std::vector<StbImage> toHSB() const;
	std::vector<StbImage> toHSL() const;
	std::vector<StbImage> toYUV() const;

	StbImage pseudoColorDensitySlicing(const std::vector<DensitySlice>& slices) const;
	StbImage pseudoColorRedistribution(
		const std::vector<uint8_t>& lutR = {},
		const std::vector<uint8_t>& lutG = {},
		const std::vector<uint8_t>& lutB = {}) const;

private:
	void normalize(int* pixel_data, uint8_t* normalized_pixel_data, int image_elements_size, int method = MEAN);

	static void rgbToHSB(float r, float g, float b, float& h, float& s, float& v);
	static void rgbToHSL(float r, float g, float b, float& h, float& s, float& l);
};
