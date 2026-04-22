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

	// ---------------------------------------------------------------
	// A) Decomposição em planos monocromáticos e recomposição
	// ---------------------------------------------------------------

	/// Retorna uma imagem em tons de cinza contendo apenas o canal R (como intensidade)
	StbImage extractChannelR() const;
	/// Retorna uma imagem em tons de cinza contendo apenas o canal G
	StbImage extractChannelG() const;
	/// Retorna uma imagem em tons de cinza contendo apenas o canal B
	StbImage extractChannelB() const;

	/// Retorna imagem colorida mostrando só a contribuição do canal R (R,0,0)
	StbImage extractChannelR_color() const;
	/// Retorna imagem colorida mostrando só a contribuição do canal G (0,G,0)
	StbImage extractChannelG_color() const;
	/// Retorna imagem colorida mostrando só a contribuição do canal B (0,0,B)
	StbImage extractChannelB_color() const;

	/// Recompõe uma imagem RGB a partir de três imagens de canal (grayscale ou color-channel)
	static StbImage recompose(const StbImage& rPlane, const StbImage& gPlane, const StbImage& bPlane);

	// ---------------------------------------------------------------
	// B) Conversão para diferentes espaços de cores
	// ---------------------------------------------------------------

	/// Converte para CMY e retorna as três componentes como imagens separadas (grayscale)
	/// O vetor retornado tem: [0]=C, [1]=M, [2]=Y
	std::vector<StbImage> toCMY() const;

	/// Converte para CMYK e retorna as quatro componentes como imagens separadas (grayscale)
	/// O vetor retornado tem: [0]=C, [1]=M, [2]=Y, [3]=K
	std::vector<StbImage> toCMYK() const;

	/// Converte para HSB (Hue-Saturation-Brightness / HSV) e retorna [0]=H, [1]=S, [2]=B
	/// H em [0,255] mapeado de [0,360), S e B em [0,255]
	std::vector<StbImage> toHSB() const;

	/// Converte para HSL (Hue-Saturation-Luminance) e retorna [0]=H, [1]=S, [2]=L
	/// H em [0,255] mapeado de [0,360), S e L em [0,255]
	std::vector<StbImage> toHSL() const;

	/// Converte para YUV e retorna [0]=Y, [1]=U, [2]=V
	/// Y em [0,255]; U e V deslocados para [0,255] (centrado em 128)
	std::vector<StbImage> toYUV() const;

	// ---------------------------------------------------------------
	// C) Pseudocolorização – Fatiamento por Densidade
	// ---------------------------------------------------------------

	/// Aplica pseudocolorização por fatiamento de densidade em imagem grayscale.
	/// Para cada pixel cujo nível de cinza cai em um intervalo [slice.low, slice.high],
	/// o pixel recebe a cor (slice.r, slice.g, slice.b).
	/// Pixels fora de qualquer faixa mantêm o tom de cinza original (r=g=b=gray).
	/// A imagem de entrada deve ser grayscale (1 ou 3/4 canais idênticos).
	StbImage pseudoColorDensitySlicing(const std::vector<DensitySlice>& slices) const;

	// ---------------------------------------------------------------
	// D) Pseudocolorização – Redistribuição de Cores
	// ---------------------------------------------------------------

	/// Redistribui as cores de uma imagem colorida de acordo com um mapa LUT de
	/// transformação por canal. Cada canal (R, G, B) é remapeado independentemente
	/// usando uma tabela de 256 entradas (LUT).
	/// Se as LUTs estiverem vazias, usa transformações senoidais padrão para
	/// enfatizar diferentes regiões espectrais (demonstração clássica de pseudocor).
	StbImage pseudoColorRedistribution(
		const std::vector<uint8_t>& lutR = {},
		const std::vector<uint8_t>& lutG = {},
		const std::vector<uint8_t>& lutB = {}) const;

private:
	void normalize(uint16_t* pixel_data, uint8_t* normalized_pixel_data, int image_elements_size, int method = MEAN);

	// Helpers
	static void rgbToHSB(float r, float g, float b, float& h, float& s, float& v);
	static void rgbToHSL(float r, float g, float b, float& h, float& s, float& l);
};