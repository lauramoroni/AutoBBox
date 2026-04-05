#include "AutoBBox.h"
#include <fstream>
#include <string>

#define ALPHA 3

AutoBBox::AutoBBox(const char* filename) : image(filename)
{
	this->filename = filename;			// Armazena o nome do arquivo
}
AutoBBox::~AutoBBox()
{
	delete[] vertices;					// Libera a memória alocada para os vértices
	vertices = nullptr;
	vertexCount = 0;
}

void AutoBBox::GeneratePolyBBox()
{
	if (!image.pixel_data) {
		return;
	}

	// Libera vértices anteriores se existirem
	if (vertices) {
		delete[] vertices;
		vertices = nullptr;
		vertexCount = 0;
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

	Point* temp_vertices = new Point[vertex_count];

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

						temp_vertices[new_vertex_index] = Point(current_column, current_line);
						new_vertex_index++;
					}

					last_direction = next_direction;
					current_index = neighbor_index;

					break;
				}
			}
		}

		interation_count++;
	} while ((current_index != starting_index) && (interation_count <= vertex_count));

	// Armazena os vértices e o contador
	vertices = temp_vertices;
	vertexCount = new_vertex_index;
}

void AutoBBox::WriteVerticesToFile()
{
	// Verifica se os vértices foram gerados
	if (!vertices || vertexCount == 0) {
		return;
	}

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
	WriteVerticesToFile(vertices_path.c_str());
}

void AutoBBox::WriteVerticesToFile(const char* filepath)
{
	// Verifica se os vértices foram gerados
	if (!vertices || vertexCount == 0) {
		return;
	}

	std::ofstream outfile(filepath);

	if (outfile.is_open()) {
		for (int i = 0; i < vertexCount; i++) {
			outfile << vertices[i].X() << " " << vertices[i].Y() << std::endl;
		}

		outfile.close();
	}
}