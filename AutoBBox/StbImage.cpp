#include "StbImage.h"

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


StbImage StbImage::sum(const StbImage& other, int method)
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

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/sum.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memória alocada para os dados intermediários
	// O new_pixel_data será liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::subtract(const StbImage& other, int method)
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

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/sub.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memória alocada para os dados intermediários
	// O new_pixel_data será liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;
	
	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::multiply(const StbImage& other, int method)
{
	// A altura e largura serão as maiores entre as duas imagens
	int height = std::max(this->height, other.height);
	int width = std::max(this->width, other.width);

	// O número de canais será o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memória para os dados da imagem resultante
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

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o método especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];
	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/mul.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memória alocada para os dados intermediários
	// O new_pixel_data será liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::divide(const StbImage& other, int method)
{
	// A altura e largura serão as maiores entre as duas imagens
	int height = std::max(this->height, other.height);
	int width = std::max(this->width, other.width);

	// O número de canais será o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memória para os dados da imagem resultante
	uint16_t* raw_new_pixel_data = new uint16_t[width * height * channels];

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			for (int c = 0; c < channels; ++c) {
				int idx_result = (y * width + x) * channels + c;
				int idx_this = (y * this->width + x) * this->channels + c;
				int idx_other = (y * other.width + x) * other.channels + c;
				uint16_t pixel_value_this = (x < this->width && y < this->height && c < this->channels) ? this->pixel_data[idx_this] : 0;
				uint16_t pixel_value_other = (x < other.width && y < other.height && c < other.channels) ? other.pixel_data[idx_other] : 0;
				raw_new_pixel_data[idx_result] = (pixel_value_other != 0) ? (pixel_value_this / pixel_value_other) : 255; // Evita divisão por zero
			}
		}
	}

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o método especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];
	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/div.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memória alocada para os dados intermediários
	// O new_pixel_data será liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::logicalAnd(const StbImage& other, int method)
{
	// A altura e largura serão as menores entre as duas imagens
	int height = std::min(this->height, other.height);
	int width = std::min(this->width, other.width);

	// O número de canais será o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memória para os dados da imagem resultante
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

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o método especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];
	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/and.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memória alocada para os dados intermediários
	// O new_pixel_data será liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::logicalOr(const StbImage& other, int method)
{
	// A altura e largura serão as maiores entre as duas imagens
	int height = std::max(this->height, other.height);
	int width = std::max(this->width, other.width);

	// O número de canais será o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memória para os dados da imagem resultante
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

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o método especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];
	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/or.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memória alocada para os dados intermediários
	// O new_pixel_data será liberado pelo destrutor da imagem resultante
	delete[] raw_new_pixel_data;

	return StbImage(width, height, channels, new_pixel_data);
}

StbImage StbImage::logicalXor(const StbImage& other, int method)
{
	// A altura e largura serão as maiores entre as duas imagens
	int height = std::max(this->height, other.height);
	int width = std::max(this->width, other.width);

	// O número de canais será o maior entre as duas imagens
	int channels = std::max(this->channels, other.channels);

	// Aloca memória para os dados da imagem resultante
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

	// Normaliza os valores dos pixels para o intervalo de 0 a 255 usando o método especificado
	uint8_t* new_pixel_data = new uint8_t[width * height * channels];
	normalize(raw_new_pixel_data, new_pixel_data, width * height * channels, method);

	// Salvar a imagem resultante para teste
	stbi_write_png("Resources/xor.png", width, height, channels, new_pixel_data, width * channels);

	// Libera a memória alocada para os dados intermediários
	// O new_pixel_data será liberado pelo destrutor da imagem resultante
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
	// Calculando novo tamanho para não cortar a imagem
	int new_width = width + (int)(height * std::abs(shx));
	int new_height = height + (int)(width * std::abs(shy));

	uint8_t* new_data = (uint8_t*)malloc(new_width * new_height * channels);
	memset(new_data, 0, new_width * new_height * channels);

	// Inverse mapping para evitar buracos
	for (int y = 0; y < new_height; ++y) {
		for (int x = 0; x < new_width; ++x) {
			// Transformação inversa do cisalhamento
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

	// Determinante simples para matriz 2x2 (ignorando translação para a inversão base)
	float det = matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
	if (std::abs(det) < 0.0001f) return StbImage(width, height, channels, new_data); // Previne divisão por zero

	// Mapeamento inverso
	for (int dest_y = 0; dest_y < height; ++dest_y) {
		for (int dest_x = 0; dest_x < width; ++dest_x) {

			// Subtrai translação
			float tx = dest_x - matrix[0][2];
			float ty = dest_y - matrix[1][2];

			// Inversa da Rotação/Escala/Cisalhamento
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

	// Rotação ao redor do centro
	int cx = width / 2;
	int cy = height / 2;

	uint8_t* new_data = (uint8_t*)malloc(width * height * channels);
	memset(new_data, 0, width * height * channels);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// Translação para o centro, rotação inversa, translação de volta
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
	} else if (method == TRUNCATE) {
		for (int i = 0; i < image_elements_size; ++i) {
			normalized_pixel_data[i] = static_cast<uint8_t>(std::min<uint16_t>(pixel_data[i], 255));
		}
	}
}