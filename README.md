# Computação Gráfica com OpenGL

Repositório de exercícios da disciplina de Computação Gráfica. Os exemplos
apresentam de forma progressiva a criação de janelas, o desenho de formas 2D e
3D, transformações geométricas, projeções, HUDs, texturas e movimentação de
câmera usando C++ e OpenGL.

## Conteúdos estudados

- criação de janelas e tratamento de eventos com GLFW;
- carregamento das funções do OpenGL com GLEW;
- desenho de primitivas 2D e 3D;
- translação, rotação e escala;
- projeções ortográfica e perspectiva;
- buffer de profundidade;
- renderização de texto e informações em HUD;
- carregamento e aplicação de texturas com `stb_image`;
- câmera em primeira pessoa controlada pelo teclado e mouse.

Os exercícios usam o pipeline fixo do OpenGL, com funções como `glBegin`,
`glVertex` e `glColor`. Essa abordagem tem finalidade didática e permite
observar os conceitos fundamentais antes da introdução de shaders e do
pipeline moderno.

## Organização das aulas

| Diretório | Conteúdo principal |
|---|---|
| `Lesson01_July_27` | Inicialização do GLFW e primeira janela OpenGL |
| `Lesson02_August_03` | Desenho de quadrado e círculo em 2D |
| `Lesson04_August_12` | Transformações, projeções e cubo 3D |
| `Lesson05_August_19` | Cubo interativo com informações em HUD |
| `Lesson06_August_26` | Cubo com HUD e ponteiros de relógio |
| `Lesson07_September_02` | Aplicação de texturas ao cubo e ao cenário |
| `Lesson08_September_09` | Movimentação de câmera com teclado e mouse |

Cada diretório contém um exercício independente. As lessons que usam texturas
também incluem os recursos necessários em uma pasta `textures`.

## Dependências

- compilador C++ com suporte a C++17;
- OpenGL;
- GLEW;
- GLFW;
- GLU.

Em distribuições baseadas em Ubuntu, instale as dependências com:

```bash
sudo apt update
sudo apt install g++ libglew-dev libglfw3-dev libglu1-mesa-dev
```

## Compilação

Cada exercício pode ser compilado separadamente. A partir da raiz do
repositório, por exemplo:

```bash
mkdir -p build
g++ -std=c++17 Lesson04_August_12/cube.cpp -o build/lesson04_cube \
  -lGLEW -lglfw -lGL -lGLU
./build/lesson04_cube
```

Nos exemplos com texturas, execute o programa a partir do diretório da lesson
para que os arquivos sejam encontrados pelos caminhos relativos:

```bash
cd Lesson08_September_09
mkdir -p ../build
g++ -std=c++17 cubeWithHudAndTextureAndMoviment.cpp \
  -o ../build/lesson08 \
  -lGLEW -lglfw -lGL -lGLU
../build/lesson08
```

## Controles

### Transformações do objeto

Estes controles são usados nos exemplos interativos das Lessons 04 a 08:

| Teclas | Função |
|---|---|
| `Esc` | Fecha a aplicação |
| `Espaço` | Restaura o estado inicial |
| `T` + setas | Translada nos eixos X e Y |
| `T` + Page Up/Down | Translada no eixo Z |
| `R` + setas | Rotaciona nos eixos X e Y |
| `R` + Page Up/Down | Rotaciona no eixo Z |
| `E` + setas | Aplica escala nos eixos X e Y |
| `E` + Page Up/Down | Aplica escala no eixo Z |

Na Lesson 04, `O` seleciona a projeção ortográfica e `P` seleciona a projeção
em perspectiva.

### Câmera da Lesson 08

| Entrada | Função |
|---|---|
| `W` / `S` | Move a câmera para frente ou para trás |
| `A` / `D` | Move a câmera lateralmente |
| Movimento do mouse | Altera a direção da câmera |

## Propósito

O projeto registra a evolução prática dos conteúdos da disciplina. Cada aula
acrescenta conceitos sobre a base dos exercícios anteriores e permite observar
como coordenadas, matrizes, projeções, profundidade, texturas e câmera afetam a
cena renderizada.
