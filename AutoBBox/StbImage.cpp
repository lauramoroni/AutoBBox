#include "StbImage.h"
#include <vector>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <algorithm>
#include <cmath> 

#define PI 3.14159265f

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

	channels = desired_channels ? desired_channels : channels;		// Se desired_channels for 0, mantÃ©m o original

	size = static_cast<size_t>(width * height * channels);
}

StbImage::StbImage(int width, int height, int channels, uint8_t* pixel_data) : width(width), height(height), channels(channels), pixel_data(pixel_data)
{
	size = static_cast<size_t>(width * height * channels);
}


StbImage::~StbImage()
{
	stbi_image_free(pixel_data);	// Libera a memÃ³ria alocada para os dados da imagem
	pixel_data = nullptr;			// Define o ponteiro de dados da imagem como nulo
	size = 0;						// Define o tamanho como zero
	width = 0;						// Define a largura como zero
	height = 0;						// Define a altura como zero
	channels = 0;					// Define o nÃºmero de canais como zero
}

StbImage::StbImage(const StbImage& other)
	: width(other.width), height(other.height), channels(other.channels), max_val_grayscale(other.max_val_grayscale), size(other.size)
{
	if (other.pixel_data) {
		pixel_data = (uint8_t*)malloc(size);
		memcpy(pixel_data, other.pixel_data, size);
	}
	else {
		pixel_data = nullptr;
	}
}

StbImage& StbImage::operator=(const StbImage& other)
{
	if (this != &other) {
		stbi_image_free(pixel_data);

		width = other.width;
		height = other.height;
		channels = other.channels;
		max_val_grayscale = other.max_val_grayscale;
		size = other.size;

		if (other.pixel_data) {
			pixel_data = (uint8_t*)malloc(size);
			memcpy(pixel_data, other.pixel_data, size);
		}
		else {
			pixel_data = nullptr;
		}
	}
	return *this;
}

StbImage::StbImage(StbImage&& other) noexcept
	: width(other.width), height(other.height), channels(other.channels), max_val_grayscale(other.max_val_grayscale), size(other.size), pixel_data(other.pixel_data)
{
	other.pixel_data = nullptr;
	other.size = 0;
	other.width = 0;
	other.height = 0;
	other.channels = 0;
}

StbImage& StbImage::operator=(StbImage&& other) noexcept
{
	if (this != &other) {
		stbi_image_free(pixel_data);

		width = other.width;
		height = other.height;
		channels = other.channels;
		max_val_grayscale = other.max_val_grayscale;
		size = other.size;
		pixel_data = other.pixel_data;

		other.pixel_data = nullptr;
		other.size = 0;
		other.width = 0;
		other.height = 0;
		other.channels = 0;
	}
	return *this;
}

StbImage StbImage::sum(const StbImage& other, int method)
{
	// A altura e largura serÃ£o as maiores entre as duas imagens
	int height = std::max(this->height, other.height);
	int width = std::max(this->width, other.width);

	// O nÃºmero de canais serÃ¡ o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memÃ³ria para os dados da imagem resultante
	uint16_t* raw_new_pixel_data = new uint16_t[width * height * channels];

	// Soma sem normalizaÃ§Ã£o (para evitar overflow, usamos uint16_t para armazenar os resultados intermediÃ¡rios)
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

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o mÃ©todo especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];

	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/sum.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memÃ³ria alocada para os dados intermediÃ¡rios
	// O new_pixel_data serÃ¡ liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::subtract(const StbImage& other, int method)
{
	// A altura e largura serÃ£o as maiores entre as duas imagens
	int height = std::max(this->height, other.height);
	int width = std::max(this->width, other.width);

	// O nÃºmero de canais serÃ¡ o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memÃ³ria para os dados da imagem resultante
	uint16_t* raw_new_pixel_data = new uint16_t[width * height * channels];

	// SubtraÃ§Ã£o sem normalizaÃ§Ã£o (para evitar underflow, usamos uint16_t para armazenar os resultados intermediÃ¡rios)
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

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o mÃ©todo especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];
	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/sub.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memÃ³ria alocada para os dados intermediÃ¡rios
	// O new_pixel_data serÃ¡ liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::multiply(const StbImage& other, int method)
{
	// A altura e largura serÃ£o as maiores entre as duas imagens
	int height = std::max(this->height, other.height);
	int width = std::max(this->width, other.width);

	// O nÃºmero de canais serÃ¡ o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memÃ³ria para os dados da imagem resultante
	uint16_t* raw_new_pixel_data = new uint16_t[width * height * channels];

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			for (int c = 0; c < channels; ++c) {
				int idx_result = (y * width + x) * channels + c;
				int idx_this = (y * this->width + x) * this->channels + c;
				int idx_other = (y * other.width + x) * other.channels + c;
				uint16_t pixel_value_this = (x < this->width && y < this->height && c < this->channels) ? this->pixel_data[idx_this] : 0;
				uint16_t pixel_value_other = (x < other.width && y < other.height && c < other.channels) ? other.pixel_data[idx_other] : 0;
				raw_new_pixel_data[idx_result] = pixel_value_this * pixel_value_other;
			}
		}
	}

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o mÃ©todo especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];
	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/mul.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memÃ³ria alocada para os dados intermediÃ¡rios
	// O new_pixel_data serÃ¡ liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::divide(const StbImage& other, int method)
{
	// A altura e largura serÃ£o as maiores entre as duas imagens
	int height = std::max(this->height, other.height);
	int width = std::max(this->width, other.width);

	// O nÃºmero de canais serÃ¡ o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memÃ³ria para os dados da imagem resultante
	uint16_t* raw_new_pixel_data = new uint16_t[width * height * channels];

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			for (int c = 0; c < channels; ++c) {
				int idx_result = (y * width + x) * channels + c;
				int idx_this = (y * this->width + x) * this->channels + c;
				int idx_other = (y * other.width + x) * other.channels + c;
				uint16_t pixel_value_this = (x < this->width && y < this->height && c < this->channels) ? this->pixel_data[idx_this] : 0;
				uint16_t pixel_value_other = (x < other.width && y < other.height && c < other.channels) ? other.pixel_data[idx_other] : 0;
				raw_new_pixel_data[idx_result] = (pixel_value_other != 0) ? (pixel_value_this / pixel_value_other) : 255; // Evita divisÃ£o por zero
			}
		}
	}

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o mÃ©todo especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];
	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/div.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memÃ³ria alocada para os dados intermediÃ¡rios
	// O new_pixel_data serÃ¡ liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::logicalAnd(const StbImage& other, int method)
{
	// A altura e largura serÃ£o as menores entre as duas imagens
	int height = std::min(this->height, other.height);
	int width = std::min(this->width, other.width);

	// O nÃºmero de canais serÃ¡ o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memÃ³ria para os dados da imagem resultante
	uint16_t* raw_new_pixel_data = new uint16_t[width * height * channels];

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			for (int c = 0; c < channels; ++c) {
				int idx_result = (y * width + x) * channels + c;
				int idx_this = (y * this->width + x) * this->channels + c;
				int idx_other = (y * other.width + x) * other.channels + c;
				uint16_t pixel_value_this = (x < this->width && y < this->height && c < this->channels) ? this->pixel_data[idx_this] : 0;
				uint16_t pixel_value_other = (x < other.width && y < other.height && c < other.channels) ? other.pixel_data[idx_other] : 0;
				raw_new_pixel_data[idx_result] = pixel_value_this & pixel_value_other;
			}
		}
	}

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o mÃ©todo especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];
	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/and.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memÃ³ria alocada para os dados intermediÃ¡rios
	// O new_pixel_data serÃ¡ liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::logicalOr(const StbImage& other, int method)
{
	// A altura e largura serÃ£o as maiores entre as duas imagens
	int height = std::max(this->height, other.height);
	int width = std::max(this->width, other.width);

	// O nÃºmero de canais serÃ¡ o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memÃ³ria para os dados da imagem resultante
	uint16_t* raw_new_pixel_data = new uint16_t[width * height * channels];

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			for (int c = 0; c < channels; ++c) {
				int idx_result = (y * width + x) * channels + c;
				int idx_this = (y * this->width + x) * this->channels + c;
				int idx_other = (y * other.width + x) * other.channels + c;
				uint16_t pixel_value_this = (x < this->width && y < this->height && c < this->channels) ? this->pixel_data[idx_this] : 0;
				uint16_t pixel_value_other = (x < other.width && y < other.height && c < other.channels) ? other.pixel_data[idx_other] : 0;
				raw_new_pixel_data[idx_result] = pixel_value_this | pixel_value_other;
			}
		}
	}

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o mÃ©todo especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];
	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/or.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memÃ³ria alocada para os dados intermediÃ¡rios
	// O new_pixel_data serÃ¡ liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::logicalXor(const StbImage& other, int method)
{
	// A altura e largura serÃ£o as maiores entre as duas imagens
	int height = std::max(this->height, other.height);
	int width = std::max(this->width, other.width);

	// O nÃºmero de canais serÃ¡ o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memÃ³ria para os dados da imagem resultante
	uint16_t* raw_new_pixel_data = new uint16_t[width * height * channels];

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			for (int c = 0; c < channels; ++c) {
				int idx_result = (y * width + x) * channels + c;
				int idx_this = (y * this->width + x) * this->channels + c;
				int idx_other = (y * other.width + x) * other.channels + c;
				uint16_t pixel_value_this = (x < this->width && y < this->height && c < this->channels) ? this->pixel_data[idx_this] : 0;
				uint16_t pixel_value_other = (x < other.width && y < other.height && c < other.channels) ? other.pixel_data[idx_other] : 0;
				raw_new_pixel_data[idx_result] = pixel_value_this ^ pixel_value_other;
			}
		}
	}

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o mÃ©todo especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];
	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/xor.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memÃ³ria alocada para os dados intermediÃ¡rios
	// O new_pixel_data serÃ¡ liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::translate(int dx, int dy) {
	uint8_t* new_data = (uint8_t*)malloc(width * height * channels);
	memset(new_data, 0, width * height * channels); // Fundo preto/transparente

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int new_x = x + dx;
			int new_y = y + dy;

			if (new_x >= 0 && new_x < width && new_y >= 0 && new_y < height) {
				for (int c = 0; c < channels; ++c) {
					new_data[(new_y * width + new_x) * channels + c] = pixel_data[(y * width + x) * channels + c];
				}
			}
		}
	}

	// salvar imagem
	stbi_write_png("Resources/translate.png", width, height, channels, new_data, width * channels);
	return StbImage(width, height, channels, new_data);
}

StbImage StbImage::reflect(bool horizontal, bool vertical) {
	uint8_t* new_data = (uint8_t*)malloc(width * height * channels);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int src_x = horizontal ? (width - 1 - x) : x;
			int src_y = vertical ? (height - 1 - y) : y;

			for (int c = 0; c < channels; ++c) {
				new_data[(y * width + x) * channels + c] = pixel_data[(src_y * width + src_x) * channels + c];
			}
		}
	}
	return StbImage(width, height, channels, new_data);
}

StbImage StbImage::shear(float shx, float shy) {
	// Calculando novo tamanho para nÃ£o cortar a imagem
	int new_width = width + (int)(height * std::abs(shx));
	int new_height = height + (int)(width * std::abs(shy));

	uint8_t* new_data = (uint8_t*)malloc(new_width * new_height * channels);
	memset(new_data, 0, new_width * new_height * channels);

	// Inverse mapping para evitar buracos
	for (int y = 0; y < new_height; ++y) {
		for (int x = 0; x < new_width; ++x) {
			// TransformaÃ§Ã£o inversa do cisalhamento
			int src_x = (int)(x - shx * y);
			int src_y = (int)(y - shy * x);

			if (src_x >= 0 && src_x < width && src_y >= 0 && src_y < height) {
				for (int c = 0; c < channels; ++c) {
					new_data[(y * new_width + x) * channels + c] = pixel_data[(src_y * width + src_x) * channels + c];
				}
			}
		}
	}
	return StbImage(new_width, new_height, channels, new_data);
}

StbImage StbImage::applyCompositeTransform(float matrix[3][3]) {

	uint8_t* new_data = (uint8_t*)malloc(width * height * channels);
	memset(new_data, 0, width * height * channels);

	// Determinante simples para matriz 2x2 (ignorando translaÃ§Ã£o para a inversÃ£o base)
	float det = matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
	if (std::abs(det) < 0.0001f) return StbImage(width, height, channels, new_data); // Previne divisÃ£o por zero

	// Mapeamento inverso
	for (int dest_y = 0; dest_y < height; ++dest_y) {
		for (int dest_x = 0; dest_x < width; ++dest_x) {

			// Subtrai translaÃ§Ã£o
			float tx = dest_x - matrix[0][2];
			float ty = dest_y - matrix[1][2];

			// Inversa da RotaÃ§Ã£o/Escala/Cisalhamento
			int src_x = (int)((matrix[1][1] * tx - matrix[0][1] * ty) / det);
			int src_y = (int)((matrix[0][0] * ty - matrix[1][0] * tx) / det);

			if (src_x >= 0 && src_x < width && src_y >= 0 && src_y < height) {
				for (int c = 0; c < channels; ++c) {
					new_data[(dest_y * width + dest_x) * channels + c] =
						pixel_data[(src_y * width + src_x) * channels + c];
				}
			}
		}
	}
	return StbImage(width, height, channels, new_data);
}

StbImage StbImage::rotate(float angle_degrees) {
	float rad = angle_degrees * PI / 180.0f;
	float cosA = std::cos(rad);
	float sinA = std::sin(rad);

	// RotaÃ§Ã£o ao redor do centro
	int cx = width / 2;
	int cy = height / 2;

	uint8_t* new_data = (uint8_t*)malloc(width * height * channels);
	memset(new_data, 0, width * height * channels);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// TranslaÃ§Ã£o para o centro, rotaÃ§Ã£o inversa, translaÃ§Ã£o de volta
			int dx = x - cx;
			int dy = y - cy;

			int src_x = (int)(dx * cosA + dy * sinA) + cx;
			int src_y = (int)(-dx * sinA + dy * cosA) + cy;

			if (src_x >= 0 && src_x < width && src_y >= 0 && src_y < height) {
				for (int c = 0; c < channels; ++c) {
					new_data[(y * width + x) * channels + c] = pixel_data[(src_y * width + src_x) * channels + c];
				}
			}
		}
	}
	return StbImage(width, height, channels, new_data);
}

StbImage StbImage::scale(float sx, float sy)
{
	int new_width = (int)(width * sx);
	int new_height = (int)(height * sy);
	uint8_t* new_data = (uint8_t*)malloc(new_width * new_height * channels);
	for (int y = 0; y < new_height; ++y) {
		for (int x = 0; x < new_width; ++x) {
			int src_x = (int)(x / sx);
			int src_y = (int)(y / sy);
			if (src_x >= 0 && src_x < width && src_y >= 0 && src_y < height) {
				for (int c = 0; c < channels; ++c) {
					new_data[(y * new_width + x) * channels + c] = pixel_data[(src_y * width + src_x) * channels + c];
				}
			}
		}
	}
	return StbImage(new_width, new_height, channels, new_data);
}

StbImage StbImage::zoomInReplication(float factor) {
	int new_width = (int)(width * factor);
	int new_height = (int)(height * factor);
	uint8_t* new_data = (uint8_t*)malloc(new_width * new_height * channels);

	for (int y = 0; y < new_height; ++y) {
		for (int x = 0; x < new_width; ++x) {
			int src_x = (int)(x / factor);
			int src_y = (int)(y / factor);

			for (int c = 0; c < channels; ++c) {
				new_data[(y * new_width + x) * channels + c] = pixel_data[(src_y * width + src_x) * channels + c];
			}
		}
	}
	return StbImage(new_width, new_height, channels, new_data);
}

StbImage StbImage::zoomInInterpolation(float factor) {
	int new_width = (int)(width * factor);
	int new_height = (int)(height * factor);
	uint8_t* new_data = (uint8_t*)malloc(new_width * new_height * channels);

	for (int y = 0; y < new_height; ++y) {
		for (int x = 0; x < new_width; ++x) {
			float src_x = x / factor;
			float src_y = y / factor;

			int x1 = (int)src_x;
			int y1 = (int)src_y;
			int x2 = std::min(x1 + 1, width - 1);
			int y2 = std::min(y1 + 1, height - 1);

			float dx = src_x - x1;
			float dy = src_y - y1;

			for (int c = 0; c < channels; ++c) {
				// Pega os 4 vizinhos
				float p1 = pixel_data[(y1 * width + x1) * channels + c];
				float p2 = pixel_data[(y1 * width + x2) * channels + c];
				float p3 = pixel_data[(y2 * width + x1) * channels + c];
				float p4 = pixel_data[(y2 * width + x2) * channels + c];

				// Interpola no eixo X
				float top = p1 * (1.0f - dx) + p2 * dx;
				float bottom = p3 * (1.0f - dx) + p4 * dx;

				// Interpola no eixo Y
				float pixel_val = top * (1.0f - dy) + bottom * dy;

				new_data[(y * new_width + x) * channels + c] = (uint8_t)std::min(std::max((int)pixel_val, 0), 255);
			}
		}
	}
	return StbImage(new_width, new_height, channels, new_data);
}

StbImage StbImage::zoomOutExclusion(int factor) {
	if (factor <= 1) return *this;

	int new_width = width / factor;
	int new_height = height / factor;
	uint8_t* new_data = (uint8_t*)malloc(new_width * new_height * channels);

	for (int y = 0; y < new_height; ++y) {
		for (int x = 0; x < new_width; ++x) {
			int src_x = x * factor;
			int src_y = y * factor;

			for (int c = 0; c < channels; ++c) {
				new_data[(y * new_width + x) * channels + c] = pixel_data[(src_y * width + src_x) * channels + c];
			}
		}
	}
	return StbImage(new_width, new_height, channels, new_data);
}

StbImage StbImage::zoomOutMean(int factor) {
	if (factor <= 1) return *this;

	int new_width = width / factor;
	int new_height = height / factor;
	uint8_t* new_data = (uint8_t*)malloc(new_width * new_height * channels);

	for (int y = 0; y < new_height; ++y) {
		for (int x = 0; x < new_width; ++x) {

			for (int c = 0; c < channels; ++c) {
				int sum = 0;
				int count = 0;

				// Percorre o bloco factor x factor na imagem original
				for (int fy = 0; fy < factor; ++fy) {
					for (int fx = 0; fx < factor; ++fx) {
						int src_x = x * factor + fx;
						int src_y = y * factor + fy;

						if (src_x < width && src_y < height) {
							sum += pixel_data[(src_y * width + src_x) * channels + c];
							count++;
						}
					}
				}
				new_data[(y * new_width + x) * channels + c] = (uint8_t)(sum / count);
			}
		}
	}
	return StbImage(new_width, new_height, channels, new_data);
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
	}
	else if (method == TRUNCATE) {
		for (int i = 0; i < image_elements_size; ++i) {
			normalized_pixel_data[i] = static_cast<uint8_t>(std::min<uint16_t>(pixel_data[i], 255));
		}
	}
}

bool StbImage::save(const char* filename) { return stbi_write_png(filename, width, height, channels, pixel_data, width * channels) != 0; }

// =============================================================================
// Helper: RGB (normalized [0,1]) -> HSB (h in [0,360), s,v in [0,1])
// =============================================================================
void StbImage::rgbToHSB(float r, float g, float b, float& h, float& s, float& v)
{
	float cmax = std::max({ r, g, b });
	float cmin = std::min({ r, g, b });
	float delta = cmax - cmin;

	v = cmax;

	if (cmax == 0.0f) { s = 0.0f; h = 0.0f; return; }
	s = delta / cmax;

	if (delta == 0.0f) { h = 0.0f; return; }

	if (cmax == r)      h = 60.0f * std::fmod((g - b) / delta, 6.0f);
	else if (cmax == g) h = 60.0f * ((b - r) / delta + 2.0f);
	else                h = 60.0f * ((r - g) / delta + 4.0f);

	if (h < 0.0f) h += 360.0f;
}

// =============================================================================
// Helper: RGB (normalized [0,1]) -> HSL (h in [0,360), s,l in [0,1])
// =============================================================================
void StbImage::rgbToHSL(float r, float g, float b, float& h, float& s, float& l)
{
	float cmax = std::max({ r, g, b });
	float cmin = std::min({ r, g, b });
	float delta = cmax - cmin;

	l = (cmax + cmin) / 2.0f;

	if (delta == 0.0f) { s = 0.0f; h = 0.0f; return; }

	s = delta / (1.0f - std::abs(2.0f * l - 1.0f));

	if (cmax == r)      h = 60.0f * std::fmod((g - b) / delta, 6.0f);
	else if (cmax == g) h = 60.0f * ((b - r) / delta + 2.0f);
	else                h = 60.0f * ((r - g) / delta + 4.0f);

	if (h < 0.0f) h += 360.0f;
}

// =============================================================================
// A) Decomposição em planos monocromáticos
// =============================================================================

// Cria uma imagem RGBA onde todos os pixels têm R=G=B=valor_do_canal, A=255
static StbImage makeGrayFromChannel(int w, int h, int ch, const uint8_t* src, int chanIdx)
{
	uint8_t* data = (uint8_t*)malloc(w * h * 4);
	for (int i = 0; i < w * h; ++i) {
		uint8_t val = (chanIdx < ch) ? src[i * ch + chanIdx] : 0;
		data[i * 4 + 0] = val;
		data[i * 4 + 1] = val;
		data[i * 4 + 2] = val;
		data[i * 4 + 3] = 255;
	}
	return StbImage(w, h, 4, data);
}

StbImage StbImage::extractChannelR() const { return makeGrayFromChannel(width, height, channels, pixel_data, 0); }
StbImage StbImage::extractChannelG() const { return makeGrayFromChannel(width, height, channels, pixel_data, 1); }
StbImage StbImage::extractChannelB() const { return makeGrayFromChannel(width, height, channels, pixel_data, 2); }

// Imagem colorida com apenas o canal especificado ativo
StbImage StbImage::extractChannelR_color() const
{
	uint8_t* data = (uint8_t*)malloc(width * height * 4);
	for (int i = 0; i < width * height; ++i) {
		data[i * 4 + 0] = (0 < channels) ? pixel_data[i * channels + 0] : 0;
		data[i * 4 + 1] = 0;
		data[i * 4 + 2] = 0;
		data[i * 4 + 3] = 255;
	}
	return StbImage(width, height, 4, data);
}

StbImage StbImage::extractChannelG_color() const
{
	uint8_t* data = (uint8_t*)malloc(width * height * 4);
	for (int i = 0; i < width * height; ++i) {
		data[i * 4 + 0] = 0;
		data[i * 4 + 1] = (1 < channels) ? pixel_data[i * channels + 1] : 0;
		data[i * 4 + 2] = 0;
		data[i * 4 + 3] = 255;
	}
	return StbImage(width, height, 4, data);
}

StbImage StbImage::extractChannelB_color() const
{
	uint8_t* data = (uint8_t*)malloc(width * height * 4);
	for (int i = 0; i < width * height; ++i) {
		data[i * 4 + 0] = 0;
		data[i * 4 + 1] = 0;
		data[i * 4 + 2] = (2 < channels) ? pixel_data[i * channels + 2] : 0;
		data[i * 4 + 3] = 255;
	}
	return StbImage(width, height, 4, data);
}

StbImage StbImage::recompose(const StbImage& rPlane, const StbImage& gPlane, const StbImage& bPlane)
{
	int w = rPlane.width;
	int h = rPlane.height;
	uint8_t* data = (uint8_t*)malloc(w * h * 4);

	for (int i = 0; i < w * h; ++i) {
		// Each plane is grayscale stored as RGBA (R=G=B=val), use first channel
		auto getVal = [&](const StbImage& plane, int pixIdx) -> uint8_t {
			if (plane.pixel_data == nullptr || pixIdx >= (int)(plane.size / plane.channels)) return 0;
			return plane.pixel_data[pixIdx * plane.channels];
			};
		data[i * 4 + 0] = getVal(rPlane, i);
		data[i * 4 + 1] = getVal(gPlane, i);
		data[i * 4 + 2] = getVal(bPlane, i);
		data[i * 4 + 3] = 255;
	}
	return StbImage(w, h, 4, data);
}

// =============================================================================
// B) Espaços de cores
// =============================================================================

std::vector<StbImage> StbImage::toCMY() const
{
	// CMY = 1 - RGB (normalizado), então C=255-R, M=255-G, Y=255-B
	auto makePlane = [&](int chanOffset) -> StbImage {
		uint8_t* data = (uint8_t*)malloc(width * height * 4);
		for (int i = 0; i < width * height; ++i) {
			uint8_t src = (chanOffset < channels) ? pixel_data[i * channels + chanOffset] : 0;
			uint8_t val = (uint8_t)(255 - src);
			data[i * 4 + 0] = val;
			data[i * 4 + 1] = val;
			data[i * 4 + 2] = val;
			data[i * 4 + 3] = 255;
		}
		return StbImage(width, height, 4, data);
		};

	std::vector<StbImage> result;
	result.push_back(makePlane(0)); // C
	result.push_back(makePlane(1)); // M
	result.push_back(makePlane(2)); // Y
	return result;
}

std::vector<StbImage> StbImage::toCMYK() const
{
	// Primeiro calcula CMY normalizado [0,1], depois K = min(C,M,Y)
	// e ajusta C'=(C-K)/(1-K), etc.
	uint8_t* cData = (uint8_t*)malloc(width * height * 4);
	uint8_t* mData = (uint8_t*)malloc(width * height * 4);
	uint8_t* yData = (uint8_t*)malloc(width * height * 4);
	uint8_t* kData = (uint8_t*)malloc(width * height * 4);

	for (int i = 0; i < width * height; ++i) {
		float r = (0 < channels) ? pixel_data[i * channels + 0] / 255.0f : 0.0f;
		float g = (1 < channels) ? pixel_data[i * channels + 1] / 255.0f : 0.0f;
		float b = (2 < channels) ? pixel_data[i * channels + 2] / 255.0f : 0.0f;

		float C = 1.0f - r;
		float M = 1.0f - g;
		float Y = 1.0f - b;
		float K = std::min({ C, M, Y });

		float Cp, Mp, Yp;
		if (K >= 1.0f) { Cp = 0.0f; Mp = 0.0f; Yp = 0.0f; }
		else {
			Cp = (C - K) / (1.0f - K);
			Mp = (M - K) / (1.0f - K);
			Yp = (Y - K) / (1.0f - K);
		}

		auto toU8 = [](float v) -> uint8_t {
			return (uint8_t)std::max(0, std::min(255, (int)(v * 255.0f + 0.5f)));
			};

		auto setGray = [&](uint8_t* d, int idx, uint8_t val) {
			d[idx * 4 + 0] = val;
			d[idx * 4 + 1] = val;
			d[idx * 4 + 2] = val;
			d[idx * 4 + 3] = 255;
			};

		setGray(cData, i, toU8(Cp));
		setGray(mData, i, toU8(Mp));
		setGray(yData, i, toU8(Yp));
		setGray(kData, i, toU8(K));
	}

	std::vector<StbImage> result;
	result.push_back(StbImage(width, height, 4, cData));
	result.push_back(StbImage(width, height, 4, mData));
	result.push_back(StbImage(width, height, 4, yData));
	result.push_back(StbImage(width, height, 4, kData));
	return result;
}

std::vector<StbImage> StbImage::toHSB() const
{
	uint8_t* hData = (uint8_t*)malloc(width * height * 4);
	uint8_t* sData = (uint8_t*)malloc(width * height * 4);
	uint8_t* vData = (uint8_t*)malloc(width * height * 4);

	for (int i = 0; i < width * height; ++i) {
		float r = (0 < channels) ? pixel_data[i * channels + 0] / 255.0f : 0.0f;
		float g = (1 < channels) ? pixel_data[i * channels + 1] / 255.0f : 0.0f;
		float b = (2 < channels) ? pixel_data[i * channels + 2] / 255.0f : 0.0f;

		float h, s, v;
		rgbToHSB(r, g, b, h, s, v);

		uint8_t hU = (uint8_t)std::max(0, std::min(255, (int)(h / 360.0f * 255.0f)));
		uint8_t sU = (uint8_t)std::max(0, std::min(255, (int)(s * 255.0f)));
		uint8_t vU = (uint8_t)std::max(0, std::min(255, (int)(v * 255.0f)));

		auto setGray = [&](uint8_t* d, int idx, uint8_t val) {
			d[idx * 4 + 0] = val; d[idx * 4 + 1] = val;
			d[idx * 4 + 2] = val; d[idx * 4 + 3] = 255;
			};
		setGray(hData, i, hU);
		setGray(sData, i, sU);
		setGray(vData, i, vU);
	}

	std::vector<StbImage> result;
	result.push_back(StbImage(width, height, 4, hData)); // H
	result.push_back(StbImage(width, height, 4, sData)); // S
	result.push_back(StbImage(width, height, 4, vData)); // B/V
	return result;
}

std::vector<StbImage> StbImage::toHSL() const
{
	uint8_t* hData = (uint8_t*)malloc(width * height * 4);
	uint8_t* sData = (uint8_t*)malloc(width * height * 4);
	uint8_t* lData = (uint8_t*)malloc(width * height * 4);

	for (int i = 0; i < width * height; ++i) {
		float r = (0 < channels) ? pixel_data[i * channels + 0] / 255.0f : 0.0f;
		float g = (1 < channels) ? pixel_data[i * channels + 1] / 255.0f : 0.0f;
		float b = (2 < channels) ? pixel_data[i * channels + 2] / 255.0f : 0.0f;

		float h, s, l;
		rgbToHSL(r, g, b, h, s, l);

		uint8_t hU = (uint8_t)std::max(0, std::min(255, (int)(h / 360.0f * 255.0f)));
		uint8_t sU = (uint8_t)std::max(0, std::min(255, (int)(s * 255.0f)));
		uint8_t lU = (uint8_t)std::max(0, std::min(255, (int)(l * 255.0f)));

		auto setGray = [&](uint8_t* d, int idx, uint8_t val) {
			d[idx * 4 + 0] = val; d[idx * 4 + 1] = val;
			d[idx * 4 + 2] = val; d[idx * 4 + 3] = 255;
			};
		setGray(hData, i, hU);
		setGray(sData, i, sU);
		setGray(lData, i, lU);
	}

	std::vector<StbImage> result;
	result.push_back(StbImage(width, height, 4, hData)); // H
	result.push_back(StbImage(width, height, 4, sData)); // S
	result.push_back(StbImage(width, height, 4, lData)); // L
	return result;
}

std::vector<StbImage> StbImage::toYUV() const
{
	// Matriz NTSC: Y = 0.299R + 0.587G + 0.114B
	//              U = -0.14713R - 0.28886G + 0.436B
	//              V =  0.615R  - 0.51499G - 0.10001B
	// U e V têm faixa negativa; deslocamos para [0,255] adicionando 128.
	uint8_t* yData = (uint8_t*)malloc(width * height * 4);
	uint8_t* uData = (uint8_t*)malloc(width * height * 4);
	uint8_t* vData = (uint8_t*)malloc(width * height * 4);

	for (int i = 0; i < width * height; ++i) {
		float r = (0 < channels) ? pixel_data[i * channels + 0] / 255.0f : 0.0f;
		float g = (1 < channels) ? pixel_data[i * channels + 1] / 255.0f : 0.0f;
		float b = (2 < channels) ? pixel_data[i * channels + 2] / 255.0f : 0.0f;

		float Y = 0.299f * r + 0.587f * g + 0.114f * b;
		float U = -0.14713f * r - 0.28886f * g + 0.436f * b;
		float V = 0.615f * r - 0.51499f * g - 0.10001f * b;

		auto clampU8 = [](float v) -> uint8_t {
			return (uint8_t)std::max(0, std::min(255, (int)(v * 255.0f + 0.5f)));
			};
		auto clampU8_shifted = [](float v) -> uint8_t {
			// shift from [-0.5, 0.5] range to [0,255]
			return (uint8_t)std::max(0, std::min(255, (int)((v + 0.5f) * 255.0f + 0.5f)));
			};

		uint8_t yU = clampU8(Y);
		uint8_t uU = clampU8_shifted(U);
		uint8_t vU = clampU8_shifted(V);

		auto setGray = [&](uint8_t* d, int idx, uint8_t val) {
			d[idx * 4 + 0] = val; d[idx * 4 + 1] = val;
			d[idx * 4 + 2] = val; d[idx * 4 + 3] = 255;
			};
		setGray(yData, i, yU);
		setGray(uData, i, uU);
		setGray(vData, i, vU);
	}

	std::vector<StbImage> result;
	result.push_back(StbImage(width, height, 4, yData)); // Y
	result.push_back(StbImage(width, height, 4, uData)); // U
	result.push_back(StbImage(width, height, 4, vData)); // V
	return result;
}

// =============================================================================
// C) Pseudocolorização – Fatiamento por Densidade
// =============================================================================

StbImage StbImage::pseudoColorDensitySlicing(const std::vector<DensitySlice>& slices) const
{
	uint8_t* data = (uint8_t*)malloc(width * height * 4);

	for (int i = 0; i < width * height; ++i) {
		// Calcula a intensidade de cinza do pixel (média dos canais RGB)
		float gray = 0.0f;
		int numColorCh = std::min(channels, 3);
		for (int c = 0; c < numColorCh; ++c) {
			gray += pixel_data[i * channels + c];
		}
		if (numColorCh > 0) gray /= numColorCh;
		uint8_t grayVal = (uint8_t)std::max(0, std::min(255, (int)gray));

		// Verifica qual fatia o pixel pertence
		bool found = false;
		for (const auto& slice : slices) {
			if (grayVal >= slice.low && grayVal <= slice.high) {
				data[i * 4 + 0] = slice.r;
				data[i * 4 + 1] = slice.g;
				data[i * 4 + 2] = slice.b;
				data[i * 4 + 3] = 255;
				found = true;
				break;
			}
		}
		if (!found) {
			// Mantém o tom de cinza original
			data[i * 4 + 0] = grayVal;
			data[i * 4 + 1] = grayVal;
			data[i * 4 + 2] = grayVal;
			data[i * 4 + 3] = 255;
		}
	}
	return StbImage(width, height, 4, data);
}

// =============================================================================
// D) Pseudocolorização – Redistribuição de Cores
// =============================================================================

StbImage StbImage::pseudoColorRedistribution(
	const std::vector<uint8_t>& lutR,
	const std::vector<uint8_t>& lutG,
	const std::vector<uint8_t>& lutB) const
{
	// Se as LUTs não forem fornecidas, gera transformações senoidais clássicas:
	// R(i) = 255 * |sin(pi * i / 256)|
	// G(i) = 255 * |sin(pi * i / 256 + pi/2)| = 255 * |cos(pi * i / 256)|
	// B(i) = 255 * |sin(pi * i / 128)|  (frequência dobrada)
	std::vector<uint8_t> effR(256), effG(256), effB(256);

	if (lutR.size() == 256) {
		effR = lutR;
	}
	else {
		for (int i = 0; i < 256; ++i) {
			float t = (float)i / 255.0f;
			effR[i] = (uint8_t)(255.0f * std::abs(std::sin(PI * t)));
		}
	}
	if (lutG.size() == 256) {
		effG = lutG;
	}
	else {
		for (int i = 0; i < 256; ++i) {
			float t = (float)i / 255.0f;
			effG[i] = (uint8_t)(255.0f * std::abs(std::cos(PI * t)));
		}
	}
	if (lutB.size() == 256) {
		effB = lutB;
	}
	else {
		for (int i = 0; i < 256; ++i) {
			float t = (float)i / 255.0f;
			effB[i] = (uint8_t)(255.0f * std::abs(std::sin(2.0f * PI * t)));
		}
	}

	uint8_t* data = (uint8_t*)malloc(width * height * 4);

	for (int i = 0; i < width * height; ++i) {
		uint8_t r = (0 < channels) ? pixel_data[i * channels + 0] : 0;
		uint8_t g = (1 < channels) ? pixel_data[i * channels + 1] : 0;
		uint8_t b = (2 < channels) ? pixel_data[i * channels + 2] : 0;

		data[i * 4 + 0] = effR[r];
		data[i * 4 + 1] = effG[g];
		data[i * 4 + 2] = effB[b];
		data[i * 4 + 3] = 255;
	}
	return StbImage(width, height, 4, data);
}