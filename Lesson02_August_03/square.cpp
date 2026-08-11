/*
 * *********************************************************************
 * File: square.cpp
 * Author: Bruno Hemann
 * Date: 05/08/2026
 * Description: Descrição do arquivo
 * *********************************************************************
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

void vReadKeyboard(GLFWwindow *pWindow) {
  if (glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(pWindow, true);
  }
}

void vDraw() {
  glBegin(GL_QUADS);
  {
    glColor3ub(255, 0, 0);
    glVertex2f(0.5, 0.5);
    glColor3ub(0, 255, 0);
    glVertex2f(-0.5, 0.5);
    glColor3ub(0, 0, 255);
    glVertex2f(-0.5, -0.5);
    glColor3ub(255, 255, 0);
    glVertex2f(0.5, -0.5);
  }
  glEnd();
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Falha ao inicializar GLFW" << std::endl;
    return EXIT_FAILURE;
  }

  GLFWwindow *pWindow = glfwCreateWindow(800, 800, "Triangulo", NULL, NULL);
  if (!pWindow) {
    std::cerr << "Falha ao criar a janela GLFW" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwSetWindowPos(pWindow, 0, 0);
  glfwMakeContextCurrent(pWindow);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Falha ao inicializar GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  while (!glfwWindowShouldClose(pWindow)) {
    glClear(GL_COLOR_BUFFER_BIT);

    vReadKeyboard(pWindow);
    vDraw();

    glfwSwapBuffers(pWindow);
    glfwPollEvents();
  }

  glfwDestroyWindow(pWindow);
  glfwTerminate();
  return EXIT_SUCCESS;
}
