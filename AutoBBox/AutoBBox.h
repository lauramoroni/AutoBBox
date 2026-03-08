#ifndef _AUTOBBOX_H_
#define _AUTOBBOX_H_


#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "Geometry.h"


struct stb_Image {
	int width;
	int height;
	int channels;
	size_t size;
	uint8_t* pixel_data;
};

enum { LEFT_UP, LEFT, LEFT_DOWN, DOWN, RIGHT_DOWN, RIGHT, RIGHT_UP, UP };

class AutoBBox
{
private:
	const char* filename = nullptr;						// Nome do arquivo da imagem
	stb_Image image = { 0, 0, 0, 0, nullptr };			// Estrutura que contém os dados da imagem
	Point* vertices = nullptr;							// Vetor de vértices do polígono
	int vertexCount = 0;								// Número de vértices

public:
	AutoBBox(const char* filename);						// Construtor que carrega a imagem
	~AutoBBox();										// Destrutor que libera a memória

	void GeneratePolyBBox();							// Gera a bounding box poligonal
	void WriteVerticesToFile();							// Escreve os vértices em um arquivo de texto (caminho padrão)
	void WriteVerticesToFile(const char* filepath);		// Escreve os vértices em um arquivo de texto (caminho customizado)
	Point* GetVertices() const { return vertices; }		// Retorna o ponteiro para os vértices
	int GetVertexCount() const { return vertexCount; }	// Retorna o número de vértices

};

#endif