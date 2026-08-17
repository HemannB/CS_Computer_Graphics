# Computação Gráfica com OpenGL

Repositório de estudos da disciplina de Computação Gráfica. O objetivo é
aprender, de forma progressiva, como criar uma janela gráfica, desenhar formas
2D e 3D e aplicar transformações e projeções usando C++ e OpenGL.

## Fundamentos estudados

- criação de janelas e tratamento de eventos com GLFW;
- carregamento das funções do OpenGL com GLEW;
- desenho de primitivas como triângulos, quadrados, círculos e cubos;
- transformações de translação, rotação e escala;
- projeções ortográfica e perspectiva;
- uso do buffer de profundidade em objetos 3D.

Os exemplos utilizam o pipeline fixo do OpenGL, com funções como `glBegin`,
`glVertex` e `glColor`. Essa abordagem é usada com finalidade didática para
apresentar os conceitos básicos antes do uso de shaders e do pipeline moderno.

## Organização

```text
Lesson01_July_27/ Introdução ao OpenGL e criação da janela
Lesson02_August_03/ Desenho de formas 2D
Lesson04_August_12/ Transformações, projeções e objetos 3D
```

## Dependências

- compilador C++ com suporte a C++17;
- OpenGL;
- GLEW;
- GLFW;
- GLU.

Em distribuições baseadas em Ubuntu, as bibliotecas podem ser instaladas com:

```bash
sudo apt install g++ libglew-dev libglfw3-dev libglu1-mesa-dev
```

## Compilação

Cada exemplo pode ser compilado separadamente. Para compilar o cubo:

```bash
g++ -std=c++17 Lesson04_August_12/cube.cpp -o cube \
  -lGLEW -lglfw -lGL -lGLU
./cube
```

Substitua `cube.cpp` pelo arquivo do exercício que deseja executar.

## Controles dos exemplos interativos

| Teclas | Função |
|---|---|
| `Esc` | Fecha a aplicação |
| `Espaço` | Restaura as transformações |
| `O` / `P` | Alterna entre projeção ortográfica e perspectiva |
| `T` + setas | Translada nos eixos X e Y |
| `T` + Page Up/Down | Translada no eixo Z |
| `R` + setas | Rotaciona nos eixos X e Y |
| `R` + Page Up/Down | Rotaciona no eixo Z |
| `E` + setas | Aplica escala nos eixos X e Y |
| `E` + Page Up/Down | Aplica escala no eixo Z |

## Propósito

Este projeto é voltado ao aprendizado e à experimentação. Cada aula acrescenta
um novo conceito sobre a base dos exemplos anteriores, permitindo observar na
prática como coordenadas, matrizes, projeções e profundidade influenciam a cena.
