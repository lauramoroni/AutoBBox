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

	if (!filename) {
		return;		// Tratamento do filename 
	}

	int width, height, original_channels;
	int desired_channels = 4;													// 4 para garantir RGBA
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
	if (!image.pixel_data) {
		return;
	}

	int vertex_count = image.width * image.height;  // Pior caso para o numero maximo de vertices

	uint8_t* src_pixel = image.pixel_data;
	int starting_index = -1;

	// Procura pelo pixel nao transparente mais superior esquerdo
	while ((src_pixel < image.pixel_data + image.size) && (starting_index == -1)) {
		if (src_pixel[ALPHA] > 0) {

			if (starting_index == -1) {
				int current_pixel = (src_pixel - image.pixel_data) / image.channels;
				starting_index = current_pixel;
			}
		}

		src_pixel += image.channels;
	}


	// Boundary tracing

	Point* vertices = new Point[vertex_count];

	int bounding_structuring_element[] = { -1, -1, 0, -1, 1, -1, 1, 0, 1, 1, 0, 1, -1, 1, -1, 0 };
	int bounding_structuring_element_size = sizeof(bounding_structuring_element) / sizeof(bounding_structuring_element[0]);

	unsigned int interation_count = 0;
	unsigned int new_vertex_index = 0;
	char last_direction = -1;
	int current_index = starting_index;
	int offset_structuring_element = 0;

	do {
		for (int i = 0; i < bounding_structuring_element_size; i += 2) {
			// Offset da ultima direcao seguida no elemento estrutural
			int offset_index = (i + offset_structuring_element * 2) % (bounding_structuring_element_size);

			int line_offset = bounding_structuring_element[offset_index];
			int column_offset = bounding_structuring_element[offset_index + 1];

			int current_element_line = (current_index / image.width) + line_offset;
			int current_element_column = (current_index % image.width) + column_offset;

			// Se o elemento estrutural estiver dentro dos limites da imagem
			if ((current_element_line >= 0) && (current_element_line < image.height) &&
				(current_element_column >= 0) && (current_element_column < image.width)) {
				int neighbor_index = (current_element_line * image.width + current_element_column);
				uint8_t* neighbor_pixel = image.pixel_data + neighbor_index * image.channels;

				if (neighbor_pixel[ALPHA] > 0) {
					char next_direction = 0;

					// Direcao do proximo pixel com base no deslocamento do elemento estrutural
					if ((line_offset == -1) && (column_offset == 0)) { next_direction = UP; offset_structuring_element = RIGHT_DOWN; }
					else if ((line_offset == -1) && (column_offset == 1)) { next_direction = RIGHT_UP; offset_structuring_element = DOWN; }
					else if ((line_offset == 0) && (column_offset == 1)) { next_direction = RIGHT;  offset_structuring_element = LEFT_DOWN; }
					else if ((line_offset == 1) && (column_offset == 1)) { next_direction = RIGHT_DOWN; offset_structuring_element = LEFT; }
					else if ((line_offset == 1) && (column_offset == 0)) { next_direction = DOWN; offset_structuring_element = LEFT_UP; }
					else if ((line_offset == 1) && (column_offset == -1)) { next_direction = LEFT_DOWN; offset_structuring_element = UP; }
					else if ((line_offset == 0) && (column_offset == -1)) { next_direction = LEFT; offset_structuring_element = RIGHT_UP; }
					else if ((line_offset == -1) && (column_offset == -1)) { next_direction = LEFT_UP; offset_structuring_element = RIGHT; }

					// Apenas adiciona um novo vertice se houver uma mudanca de direcao
					if (next_direction != last_direction) {
						int current_column = current_index % image.width;
						int current_line = current_index / image.width;

						current_column -= image.width / 2;
						current_line -= image.height / 2;

						vertices[new_vertex_index] = Point(current_column, current_line);
						new_vertex_index++;
					}

					last_direction = next_direction;
					current_index = neighbor_index;

					break;
				}
			}
		}

		interation_count++;
	} while ((current_index != starting_index) && (interation_count <= vertex_count * 2));


	// Output em arquivo de texto
	
	std::string base_name;
	if (this->filename) {
		base_name = this->filename;
		size_t last_slash = base_name.find_last_of("/\\");

		// Remove o caminho do diretorio, se presente
		if (std::string::npos != last_slash) {
			base_name.erase(0, last_slash + 1);
		}

		// Remove a extensao do arquivo, se presente
		size_t last_dot = base_name.find_last_of(".");
		if (std::string::npos != last_dot) {
			base_name.erase(last_dot);
		}
	}

	std::string vertices_path = "Resources/bbox/" + base_name + "_vertices.txt";
	std::ofstream outfile(vertices_path);

	if (outfile.is_open()) {
		for (int i = 0; i < new_vertex_index; i++) {
			outfile << vertices[i].X() << " " << vertices[i].Y() << std::endl;
		}

		outfile.close();
	}

	delete[] vertices;
}