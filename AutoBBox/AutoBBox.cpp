#include "AutoBBox.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


AutoBBox::AutoBBox(const char* filename)
{
	filename = filename;																					// Armazena o nome do arquivo da imagem

	// Tratamento do filename e tipo de imagem
	if (!filename || !strstr(filename, ".png")) {
		return;																							// Se o nome do arquivo for nulo ou não contiver ".png", retorna sem fazer nada
	}

	int width, height, original_channels;
	int desired_channels = 0;																				// 0 para manter os canais originais
	uint8_t *img = stbi_load(filename, &width, &height, &original_channels, desired_channels);

	if (img == nullptr) {
		return;
	}

	int channels = desired_channels ? desired_channels : original_channels;									// Se desired_channels for 0, mantém o original
	image = { width, height, original_channels, static_cast<size_t>(width * height * channels), img };
}

AutoBBox::~AutoBBox()
{
	stbi_image_free(image.pixel_data);														// Libera a memória alocada para os dados da imagem
	image.pixel_data = nullptr;																// Define o ponteiro de dados da imagem como nulo
	image.size = 0;																			// Define o tamanho como zero
	image.width = 0;																		// Define a largura como zero
	image.height = 0;																		// Define a altura como zero
	image.channels = 0;																		// Define o número de canais como zero

	delete filename;
}

void AutoBBox::generate_binarized_image()
{
	if (!image.pixel_data) {												// Verifica se os dados da imagem são válidos e se tem pelo menos 4 canais (RGBA)
		return;																					// Se não for válido, retorna sem fazer nada
	}

	int binary_channels = 2;
	size_t binary_size = image.width * image.height * binary_channels;
	uint8_t* binary_data = new uint8_t[binary_size];

	for (uint8_t* src = image.pixel_data, *bin = binary_data; src < image.pixel_data + image.size; src += image.channels, bin += binary_channels) {
		*bin = src[3] ? 255 : 0;																	// Se o canal alfa for diferente de zero, define o pixel como branco (255), caso contrário, preto (0)
		*(bin + 1) = src[3];
	}

	stbi_write_png("binarized_image.png", image.width, image.height, binary_channels, binary_data, image.width * binary_channels);	// Salva a imagem binarizada como PNG
}