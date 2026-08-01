/*
 * *********************************************************************
 * File: main.cpp
 * Author: Bruno Hemann
 * Date: 31/07/2026
 * Description: Um simples Hello World em OpenGL(ecrã fundo azul)
 * *********************************************************************
 */

// Disponibiliza as funções modernas do OpenGL.
#include <GL/glew.h>
// Fornece os recursos para criar a janela e tratar seus eventos.
#include <GLFW/glfw3.h>
// Permite exibir mensagens de erro no terminal com std::cerr.
#include <iostream>

int main() {
  // Inicializa a biblioteca GLFW antes de usar qualquer recurso dela.
  if (!glfwInit()) {
    // Encerra o programa com uma mensagem caso a inicialização falhe.
    std::cerr << "Falha na inicialização do GLFW" << std::endl;
    return EXIT_FAILURE;
  }

  // Cria uma janela de 800 x 800 pixels com um contexto OpenGL associado.
  GLFWwindow *pWindow =
      glfwCreateWindow(800, 800, "Hello World OpenGL", NULL, NULL);

  // Verifica se a janela foi criada corretamente.
  if (!pWindow) {
    std::cerr << "Falha na criação da janela GLFW" << std::endl;
    // Libera os recursos que a GLFW já havia reservado.
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Posiciona o canto superior esquerdo da janela no ponto (0, 0) da tela.
  glfwSetWindowPos(pWindow, 0, 0);

  // Torna o contexto da janela atual para as próximas chamadas do OpenGL.
  glfwMakeContextCurrent(pWindow);

  // Inicializa o GLEW para carregar os ponteiros das funções do OpenGL.
  if (glewInit() != GLEW_OK) {
    std::cerr << "Falha na inicialização do GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  // Define a cor usada para limpar o fundo no formato RGBA.
  // As componentes RGB são divididas por 255 para ficarem entre 0.0 e 1.0;
  // o valor 1.0 no canal alfa deixa a cor totalmente visível.
  glClearColor(25.0f / 255.0f, 118.0f / 255.0f, 210.0f / 255.0f, 1.0f);

  // Mantém a aplicação em execução enquanto o usuário não fechar a janela.
  while (!glfwWindowShouldClose(pWindow)) {
    // Limpa o buffer de cores e preenche a janela com a cor definida acima.
    glClear(GL_COLOR_BUFFER_BIT);

    // Exibe o quadro que acabou de ser desenhado (troca os buffers da janela).
    glfwSwapBuffers(pWindow);

    // Processa eventos pendentes, como teclado, mouse e fechamento da janela.
    glfwPollEvents();
  }

  // Destrói a janela e libera os recursos associados a ela.
  glfwDestroyWindow(pWindow);

  // Finaliza a GLFW e libera os demais recursos mantidos pela biblioteca.
  glfwTerminate();

  // Informa ao sistema operacional que o programa terminou com sucesso.
  return EXIT_SUCCESS;
}
