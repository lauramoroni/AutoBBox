#include "AutoBBox.h"
#include <fstream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define ALPHA 3


AutoBBox::AutoBBox(const char* filename)
{
	this->filename = filename;														// Armazena o nome do arquivo da imagem

	if (!filename || !strstr(filename, ".png")) {
		return;		// Tratamento do filename e tipo de imagem
	}

	int width, height, original_channels;
	int desired_channels = 0;													// 0 para manter os canais originais
	uint8_t* img = stbi_load(filename, &width, &height, &original_channels, desired_channels);

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

	int edge_structuring_element[] = { -1, 0, 0, -1, 0, 0, 0, 1, 1, 0 };
	char edge_structuring_element_size = sizeof(edge_structuring_element) / sizeof(edge_structuring_element[0]);	// Calcula o tamanho do elemento estruturante

	int vertex_count = 0;

	int starting_index = -1;

	for (uint8_t* src_pixel = image.pixel_data, *bin_pixel = binary_data; src_pixel < image.pixel_data + image.size; src_pixel += image.channels, bin_pixel += binary_channels) {
		int min_pixel_alpha = 255;

		int current_pixel = (src_pixel - image.pixel_data) / image.channels;	// Calcula o índice do pixel atual

		for (int i = 0; i < edge_structuring_element_size; i += 2) {
			int line_offset = edge_structuring_element[i];
			int column_offset = edge_structuring_element[i + 1];

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
		*bin_pixel = 0;  // Cor
		*(bin_pixel + 1) = 0;  // Alfa

		// Se o pixel original tiver alfa visível e o erodido não, então o pixel faz parte da borda
		if (min_pixel_alpha <= 0 && src_pixel[ALPHA] > 0) {
			*(bin_pixel + 1) = 255;
			vertex_count++;

			if (starting_index == -1) {
				starting_index = current_pixel;
			}
		}
	}

	if (!vertex_count)
		return;

	std::string base_name;
	if (this->filename) {
		base_name = this->filename;
		// Remove o caminho do diretório, se houver
		size_t last_slash = base_name.find_last_of("/\\");
		if (std::string::npos != last_slash) {
			base_name.erase(0, last_slash + 1);
		}
		// Remove a extensão do arquivo, se houver
		size_t last_dot = base_name.find_last_of(".");
		if (std::string::npos != last_dot) {
			base_name.erase(last_dot);
		}
	}

	std::string binarized_path = "Resources/bbox/" + base_name + "_binarized.png";
	stbi_write_png(binarized_path.c_str(), image.width, image.height, binary_channels, binary_data, image.width * binary_channels);	// Salva a imagem binarizada como PNG

	Point* vertices = new Point[vertex_count];

	int bounding_structuring_element[] = { -1, -1, 0, -1, 1, -1, 1, 0, 1, 1, 0, 1, -1, 1, -1, 0 };
	int bounding_structuring_element_size = sizeof(bounding_structuring_element) / sizeof(bounding_structuring_element[0]);

	// Variáveis de controle
	unsigned int interation_count = 0;
	unsigned int new_vertex_index = 0;
	char last_direction = -1;
	int current_index = starting_index;

	do {
		// Procura pelo próximo vértice na borda usando o elemento estruturante
		for (int i = 0; i < bounding_structuring_element_size; i += 2) {
			int line_offset = bounding_structuring_element[i];
			int column_offset = bounding_structuring_element[i + 1];

			int current_element_line = (current_index / image.width) + line_offset;
			int current_element_column = (current_index % image.width) + column_offset;

			if ((current_element_line >= 0) && (current_element_line < image.height) &&				// Verifica se o pixel está dentro dos limites
				(current_element_column >= 0) && (current_element_column < image.width)) {
				int neighbor_index = (current_element_line * image.width + current_element_column);	// Calcula o índice do pixel vizinho
				uint8_t* neighbor_pixel = binary_data + neighbor_index * binary_channels;			// Obtém o ponteiro para o pixel vizinho

				if (neighbor_pixel[1] == 255) {	    // Se o pixel vizinho for parte da borda
					char next_direction = 0;

					if ((line_offset == -1) && (column_offset == 0)) next_direction = UP;
					else if ((line_offset == -1) && (column_offset == 1)) next_direction = RIGHT_UP;
					else if ((line_offset == 0) && (column_offset == 1)) next_direction = RIGHT;
					else if ((line_offset == 1) && (column_offset == 1)) next_direction = RIGHT_DOWN;
					else if ((line_offset == 1) && (column_offset == 0)) next_direction = DOWN;
					else if ((line_offset == 1) && (column_offset == -1)) next_direction = LEFT_DOWN;
					else if ((line_offset == 0) && (column_offset == -1)) next_direction = LEFT;
					else if ((line_offset == -1) && (column_offset == -1)) next_direction = LEFT_UP;

					if (next_direction != last_direction) {
						int current_column = current_index % image.width;
						int current_line = current_index / image.width;

						current_column -= image.width / 2;
						current_line -= image.height / 2;

						vertices[new_vertex_index] = Point(current_column, current_line);	// Armazena o vértice encontrado
						new_vertex_index++;
					}

					last_direction = next_direction;

					uint8_t* current_pixel = binary_data + current_index * binary_channels;
					current_pixel[1] = 0;            // Marca o pixel como visitado, definindo o canal alfa como 0

					current_index = neighbor_index;	 // Atualiza o índice atual para o índice do vizinho

					break;	                         // Sai do loop para procurar o próximo vértice
				}
			}
		}
		interation_count++;
	} while ((current_index != starting_index) && (interation_count <= vertex_count));


	std::string vertices_path = "Resources/bbox/" + base_name + "_vertices.txt";
	std::ofstream outfile(vertices_path);

	if (outfile.is_open()) {
		for (int i = 0; i < new_vertex_index; i++) {
			outfile << "Point(" << vertices[i].X() << ", " << vertices[i].Y() << ")," << std::endl;
		}
		// quantidade de vertices
		outfile << new_vertex_index << std::endl;
		outfile.close();
	}


	delete[] vertices;
	delete[] binary_data;
}