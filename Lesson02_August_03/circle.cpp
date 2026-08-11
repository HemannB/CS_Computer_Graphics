/*
 * *********************************************************************
 * File: circle.cpp
 * Author: Bruno Hemann
 * Date: 05/08/2026
 * Description: Descrição do arquivo
 * *********************************************************************
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>

const double PI = 3.1415926535897932;

void vReadKeyboard(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void vDraw(float dCenterX, float dCenterY, float dRadius, int iSegments) {
  glBegin(GL_TRIANGLES);

  for (int i = 0; i < iSegments; i++) {
    float dCurrentAngle = 2.0f * PI * static_cast<float>(i) / iSegments;

    float dNextAngle = 2.0f * PI * static_cast<float>(i + 1) / iSegments;

    float dCurrentX = dCenterX + dRadius * std::cos(dCurrentAngle);
    float dCurrentY = dCenterY + dRadius * std::sin(dCurrentAngle);

    float dNextX = dCenterX + dRadius * std::cos(dNextAngle);
    float dNextY = dCenterY + dRadius * std::sin(dNextAngle);

    glVertex2f(dCenterX, dCenterY);

    glVertex2f(dCurrentX, dCurrentY);

    glVertex2f(dNextX, dNextY);
  }

  glEnd();
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Falha ao inicializar GLFW" << std::endl;
    return EXIT_FAILURE;
  }

  GLFWwindow *window = glfwCreateWindow(800, 800, "Triangulo", NULL, NULL);
  if (!window) {
    std::cerr << "Falha ao criar a janela GLFW" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwSetWindowPos(window, 0, 0);
  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Falha ao inicializar GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    vReadKeyboard(window);
    vDraw(0.0f, 0.0f, 0.5f, 360);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}
