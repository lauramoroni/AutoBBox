#include "AutoBBox.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define ALPHA 3


AutoBBox::AutoBBox(const char* filename)
{
	filename = filename;														// Armazena o nome do arquivo da imagem

	if (!filename || !strstr(filename, ".png")) {
		return;		// Tratamento do filename e tipo de imagem
	}

	int width, height, original_channels;
	int desired_channels = 0;													// 0 para manter os canais originais
	uint8_t *img = stbi_load(filename, &width, &height, &original_channels, desired_channels);

	if (img == nullptr) {
		return;
	}

	int channels = desired_channels ? desired_channels : original_channels;		// Se desired_channels for 0, mantém o original
	image = { width, height, original_channels, static_cast<size_t>(width * height * channels), img };
}

AutoBBox::~AutoBBox()
{
	stbi_image_free(image.pixel_data);	// Libera a memória alocada para os dados da imagem
	image.pixel_data = nullptr;			// Define o ponteiro de dados da imagem como nulo
	image.size = 0;						// Define o tamanho como zero
	image.width = 0;					// Define a largura como zero
	image.height = 0;					// Define a altura como zero
	image.channels = 0;					// Define o número de canais como zero
}

void AutoBBox::generate_poly_bbox()
{
	if (!image.pixel_data) {	// Verifica se os dados da imagem são válidos e se tem pelo menos 4 canais (RGBA)
		return;					// Se não for válido, retorna sem fazer nada
	}

	int binary_channels = 2;
	size_t binary_size = image.width * image.height * binary_channels;
	uint8_t* binary_data = new uint8_t[binary_size];

	int structuring_element[] = { -1, 0, 0, -1, 0, 0, 0, 1, 1, 0 };
	char structuring_element_size = sizeof(structuring_element) / sizeof(structuring_element[0]);	// Calcula o tamanho do elemento estruturante

	int vertex_count = 0;

	for (uint8_t* src_pixel = image.pixel_data, *bin_pixel = binary_data; src_pixel < image.pixel_data + image.size; src_pixel += image.channels, bin_pixel += binary_channels) {
		int min_pixel_alpha = 255;

		int current_pixel = (src_pixel - image.pixel_data) / image.channels;	// Calcula o índice do pixel atual

		for (int i = 0; i < structuring_element_size; i += 2) {
			int line_offset = structuring_element[i];
			int column_offset = structuring_element[i + 1];

			int current_element_line = (current_pixel / image.width) + line_offset;
			int current_element_column = (current_pixel % image.width) + column_offset;
			
			if ((current_element_line >= 0) && (current_element_line < image.height) &&									// Verifica se o pixel está dentro dos limites
				(current_element_column >= 0) && (current_element_column < image.width)) {

				int neighbor_index = (current_element_line * image.width + current_element_column) * image.channels;	// Calcula o índice do pixel vizinho
				uint8_t* neighbor_pixel = image.pixel_data + neighbor_index;											// Obtém o ponteiro para o pixel vizinho

				if (neighbor_pixel[ALPHA] < min_pixel_alpha) {																		
					min_pixel_alpha = neighbor_pixel[ALPHA];															// Atualiza o valor mínimo do canal alfa
				}
			}
		}

		// Por padrão, o pixel será preto e transparente
		*bin_pixel = 0;
		*(bin_pixel + 1) = 0;

		// Se o pixel original tiver alfa visível e o erodido não, então o pixel faz parte da borda
		if (min_pixel_alpha <= 0 && src_pixel[ALPHA] > 0) {  
			*(bin_pixel + 1) = 255;
			vertex_count++;
		}
	}

	if (!vertex_count)
		return;

	/*Point* vertices = new Point[vertex_count];							// Cria um vetor de pontos para armazenar os vértices
	for (uint8_t* bin_pixel = binary_data; bin_pixel < binary_data + binary_size; bin_pixel += binary_channels) {
		int current_pixel = (bin_pixel - binary_data) / binary_channels;	// Calcula o índice do pixel atual
		if (bin_pixel[ALPHA] > 0) {											// Verifica se o pixel atual é visível
			int column = current_pixel % image.width;
			int line = current_pixel / image.width;
			vertices[vertex_count++] = Point(column, line);					// Adiciona o ponto ao vetor de vértices
		}
	}*/

	stbi_write_png("binarized_image.png", image.width, image.height, binary_channels, binary_data, image.width * binary_channels);	// Salva a imagem binarizada como PNG

	delete[] binary_data;
	//return vertices;
}