# AutoBBox

O AutoBBox é uma ferramenta em C++ projetada para gerar automaticamente uma bounding box poligonal a partir de uma imagem PNG com canal alfa (transparência).

## Índice
- [Proposta](#proposta)
- [Tecnologias Utilizadas](#tecnologias-utilizadas)
- [Passo a Passo do Algoritmo](#passo-a-passo-do-algoritmo)

## Proposta

Em desenvolvimento de jogos e aplicações gráficas, é comum a necessidade de definir áreas de colisão ou interação para sprites e outros elementos visuais. Realizar este processo manualmente, definindo cada vértice da área de colisão, é trabalhoso e impreciso.

O **AutoBBox** resolve este problema ao analisar uma imagem `.png` e identificar a forma exata do conteúdo visível, onde o canal alfa é maior que zero. A partir dessa análise, ele gera uma lista de vértices que formam um polígono conciso e preciso ao redor do objeto, otimizando o processo de criação de bounding boxes.

## Tecnologias Utilizadas

- **Linguagem:** C++
- **Game Engine:** Baseada na [Volt-Engine](https://github.com/JudsonSS/Volt-Engine).
- **Bibliotecas:**
  - **stb_image.h:** Utilizada para carregar e decodificar a imagem PNG de entrada para um buffer de pixels em memória.
  - **stb_image_write.h:** Utilizada para salvar a imagem intermediária com a borda detectada, auxiliando na depuração do algoritmo.
  - **iostream / fstream:** Bibliotecas padrão do C++ para manipulação de arquivos, usadas para exportar a lista final de vértices.

## Passo a Passo do Algoritmo

O processo para extrair os vértices da bounding box pode ser dividido em duas etapas principais: **Extração de Borda** e **Rastreamento de Vértices**. O fluxograma abaixo ilustra o fluxo geral da operação:

![AutoBBox - Jam](https://github.com/user-attachments/assets/9c1ec358-7c43-42d6-b5e7-a94bed708682)

---

### 1. Carregamento e Preparação da Imagem
- O programa inicia carregando uma imagem no formato `.png` especificada pelo usuário.
- Utilizando a biblioteca `stb_image`, a imagem é decodificada em uma matriz de pixels. O algoritmo depende fundamentalmente do canal alfa (**A**) do padrão RGBA para funcionar.

### 2. Binarização e Extração da Borda
- O algoritmo percorre cada pixel da imagem original e, com o auxílio de um [structuring element](https://towardsdatascience.com/understanding-morphological-image-processing-and-its-operations-7bcf1ed11756/), compara o valor do canal alfa do pixel atual com o de seus vizinhos.
<img width="150" height="150" alt="se_border" src="https://github.com/user-attachments/assets/93478e25-954d-4ec1-914d-d1f112592279" />

- Um pixel é considerado parte da **borda** se ele for visível (alfa > 0) e pelo menos um de seus vizinhos diretos for totalmente transparente (alfa = 0). Este processo é análogo a uma operação de [erosão morfológica](https://neubias.github.io/training-resources/filter_morphological/index.html) seguida pela [subtração](https://homepages.inf.ed.ac.uk/rbf/HIPR2/pixsub.htm) da imagem original.
- Ao final desta etapa, temos uma representação de todos os pixels que compõem o contorno do objeto. O primeiro pixel de borda encontrado é salvo como o **ponto de partida** para o rastreamento da borda.

### 3. Rastreamento da Borda e Identificação dos Vértices
- A partir do ponto de partida, o algoritmo começa a "caminhar" pelo contorno da imagem para encontrar a ordem correta dos vértices para construção do polígono.
- É utilizado um segundo **structuring element**, que verifica os 8 pixels vizinhos em uma ordem predefinida (sentido horário ou anti-horário) para encontrar o próximo pixel de borda conectado.
<img width="150" height="150" alt="se_contour" src="https://github.com/user-attachments/assets/81e308c5-1730-4db7-af25-60bbedf29851" />

- Um ponto chave do algoritmo é que um novo **vértice só é salvo quando a direção do movimento muda**. Isso garante que apenas as "quinas" do objeto sejam salvas, criando um polígono otimizado em vez de um ponto para cada pixel da borda.
- Cada pixel da borda que já foi processado é marcado como "visitado", tendo seu valor de alfa zerado na imagem binária.

### 4. Finalização e Exportação
- O processo de rastreamento termina quando o algoritmo retorna ao **ponto de partida** original.
- A lista de vértices coletados, que agora representa a bounding box poligonal do objeto, é exportada para um arquivo de texto (`vertices.txt`).
