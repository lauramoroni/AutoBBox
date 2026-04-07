#include "StbImage.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <algorithm>

StbImage::StbImage(const char* filename)
{
	if (!filename) {
		return;		// Tratamento do filename 
	}

	int desired_channels = 4;													// 4 para garantir RGBA
	
	pixel_data = stbi_load(filename, &width, &height, &channels, desired_channels);

	if (pixel_data == nullptr) {
		return;
	}

	channels = desired_channels ? desired_channels : channels;		// Se desired_channels for 0, mantém o original
	
	size = static_cast<size_t>(width * height * channels);
}

StbImage::StbImage(int width, int height, int channels, uint8_t* pixel_data): width(width), height(height), channels(channels), pixel_data(pixel_data)
{
	size = static_cast<size_t>(width * height * channels);
}


StbImage::~StbImage()
{
	stbi_image_free(pixel_data);	// Libera a memória alocada para os dados da imagem
	pixel_data = nullptr;			// Define o ponteiro de dados da imagem como nulo
	size = 0;						// Define o tamanho como zero
	width = 0;						// Define a largura como zero
	height = 0;						// Define a altura como zero
	channels = 0;					// Define o número de canais como zero
}


StbImage StbImage::sum(const StbImage& other, int method = MEAN)
{
	// A altura e largura serão as maiores entre as duas imagens
	int height = std::max(this->height, other.height);
	int width = std::max(this->width, other.width);

	// O número de canais será o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memória para os dados da imagem resultante
	uint16_t* raw_new_pixel_data = new uint16_t[width * height * channels];

	// Soma sem normalização (para evitar overflow, usamos uint16_t para armazenar os resultados intermediários)
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			for (int c = 0; c < channels; ++c) {
				int idx_result = (y * width + x) * channels + c;

				int idx_this = (y * this->width + x) * this->channels + c;
				int idx_other = (y * other.width + x) * other.channels + c;

				uint16_t pixel_value_this = (x < this->width && y < this->height && c < this->channels) ? this->pixel_data[idx_this] : 0;
				uint16_t pixel_value_other = (x < other.width && y < other.height && c < other.channels) ? other.pixel_data[idx_other] : 0;

				raw_new_pixel_data[idx_result] = pixel_value_this + pixel_value_other;
			}
		}
	}

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o método especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];

	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Libera a memória alocada para os dados intermediários
	// O new_pixel_data será liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::subtract(const StbImage& other, int method = MEAN)
{
	// A altura e largura serão as maiores entre as duas imagens
	int height = std::max(this->height, other.height);
	int width = std::max(this->width, other.width);

	// O número de canais será o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memória para os dados da imagem resultante
	uint16_t* raw_new_pixel_data = new uint16_t[width * height * channels];

	// Subtração sem normalização (para evitar underflow, usamos uint16_t para armazenar os resultados intermediários)
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			for (int c = 0; c < channels; ++c) {
				int idx_result = (y * width + x) * channels + c;

				int idx_this = (y * this->width + x) * this->channels + c;
				int idx_other = (y * other.width + x) * other.channels + c;

				uint16_t pixel_value_this = (x < this->width && y < this->height && c < this->channels) ? this->pixel_data[idx_this] : 0;
				uint16_t pixel_value_other = (x < other.width && y < other.height && c < other.channels) ? other.pixel_data[idx_other] : 0;

				raw_new_pixel_data[idx_result] = (pixel_value_this > pixel_value_other) ? (pixel_value_this - pixel_value_other) : 0;
			}
		}
	}

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o método especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];
	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Libera a memória alocada para os dados intermediários
	// O new_pixel_data será liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;
	
	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::multiply(const StbImage& other, int method = MEAN)
{
}

StbImage StbImage::divide(const StbImage& other, int method = MEAN)
{
}

StbImage StbImage::logicalAnd(const StbImage& other, int method = MEAN)
{
}

StbImage StbImage::logicalOr(const StbImage& other, int method = MEAN)
{
}

StbImage StbImage::logicalXor(const StbImage& other, int method = MEAN)
{
}


void StbImage::normalize(uint16_t* pixel_data, uint8_t* normalized_pixel_data, int image_elements_size, int method)
{
	if (method == MEAN) {
		// Get the maximum pixel value in the pixel_data array
		uint16_t max_pixel_value = 0;
		for (int i = 0; i < image_elements_size; ++i) {
			if (pixel_data[i] > max_pixel_value) {
				max_pixel_value = pixel_data[i];
			}
		}

		// Normalize the pixel values to the range [0, 255] using the mean method
		for (int i = 0; i < image_elements_size; ++i) {
			normalized_pixel_data[i] = static_cast<uint8_t>((static_cast<float>(pixel_data[i]) / max_pixel_value) * 255);
		}
	} else if (method == TRUNCATE) {
		for (int i = 0; i < image_elements_size; ++i) {
			normalized_pixel_data[i] = static_cast<uint8_t>(std::min<uint16_t>(pixel_data[i], 255));
		}
	}
}