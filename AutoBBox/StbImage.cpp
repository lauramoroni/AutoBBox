#include "StbImage.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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
StbImage::~StbImage()
{
	stbi_image_free(pixel_data);	// Libera a memória alocada para os dados da imagem
	pixel_data = nullptr;			// Define o ponteiro de dados da imagem como nulo
	size = 0;						// Define o tamanho como zero
	width = 0;						// Define a largura como zero
	height = 0;						// Define a altura como zero
	channels = 0;					// Define o número de canais como zero
}
StbImage StbImage::operator+(const StbImage& other)
{
	return;
}
StbImage StbImage::operator-(const StbImage& other)
{
}
StbImage StbImage::operator*(const StbImage& other)
{
}
StbImage StbImage::operator/(const StbImage& other)
{
}
StbImage StbImage::operator&(const StbImage& other)
{
}
StbImage StbImage::operator|(const StbImage& other)
{
}
StbImage StbImage::operator^(const StbImage& other)
{
}
