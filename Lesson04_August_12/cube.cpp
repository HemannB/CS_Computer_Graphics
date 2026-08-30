/*
 * *********************************************************************
 * File: cube.cpp
 * Author: Bruno Hemann
 * Date: 12/08/2026
 * Description: Cubo 3D com projeções, translação, rotação e escala.
 * *********************************************************************
 */

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>

const int kWindowWidth = 800;
const int kWindowHeight = 800;

bool orthographic_view = true;
const GLfloat kOrthographicLimitX = 6;
const GLfloat kOrthographicLimitY = 6;
const GLfloat kOrthographicLimitZ = 20;

const GLfloat kPerspectiveFieldOfViewAngle = 45;
const GLfloat kPerspectiveNearZ = 0.1f;
const GLfloat kPerspectiveFarZ = 100;
const GLfloat kPerspectiveTranslateZ = 16;

const GLfloat kTranslateLimit = 3;
const GLfloat kDefaultTranslate = 0;
GLfloat translate_increment = 0.1f;
GLfloat translate = kDefaultTranslate;
GLfloat translateY = kDefaultTranslate;
GLfloat translateZ = kDefaultTranslate;

const GLfloat kRotateAngleLimit = 360;
const GLfloat kDefaultRotateAngle = 0;
GLfloat rotate_angle_increment = 1.0f;
GLfloat rotate_angle = kDefaultRotateAngle;
GLfloat rotate_angleY = kDefaultRotateAngle;
GLfloat rotate_angleZ = kDefaultRotateAngle;

const GLfloat kScaleMin = 0.1f;
const GLfloat kScaleMax = 5.0f;
const GLfloat kDefaultScale = 1.0f;
GLfloat scale_increment = 0.1f;
GLfloat scaleX = kDefaultScale;
GLfloat scaleY = kDefaultScale;
GLfloat scaleZ = kDefaultScale;

void keyboard_read(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    translate = kDefaultTranslate;
    translateY = kDefaultTranslate;
    translateZ = kDefaultTranslate;
    rotate_angle = kDefaultRotateAngle;
    rotate_angleY = kDefaultRotateAngle;
    rotate_angleZ = kDefaultRotateAngle;
    scaleX = kDefaultScale;
    scaleY = kDefaultScale;
    scaleZ = kDefaultScale;
  }

  if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
    orthographic_view = true;
  }

  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    orthographic_view = false;
  }

  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      translate -= translate_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      translate += translate_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      translateY += translate_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      translateY -= translate_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
      translateZ += translate_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
      translateZ -= translate_increment;
    }
  }

  if (translate > kTranslateLimit) {
    translate = kTranslateLimit;
  } else if (translate < -kTranslateLimit) {
    translate = -kTranslateLimit;
  }
  if (translateY > kTranslateLimit) {
    translateY = kTranslateLimit;
  } else if (translateY < -kTranslateLimit) {
    translateY = -kTranslateLimit;
  }
  if (translateZ > kTranslateLimit) {
    translateZ = kTranslateLimit;
  } else if (translateZ < -kTranslateLimit) {
    translateZ = -kTranslateLimit;
  }

  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      rotate_angle += rotate_angle_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      rotate_angle -= rotate_angle_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      rotate_angleY += rotate_angle_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      rotate_angleY -= rotate_angle_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
      rotate_angleZ += rotate_angle_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
      rotate_angleZ -= rotate_angle_increment;
    }
  }

  if (rotate_angle >= kRotateAngleLimit ||
      rotate_angle <= -kRotateAngleLimit) {
    rotate_angle = kDefaultRotateAngle;
  }
  if (rotate_angleY >= kRotateAngleLimit ||
      rotate_angleY <= -kRotateAngleLimit) {
    rotate_angleY = kDefaultRotateAngle;
  }
  if (rotate_angleZ >= kRotateAngleLimit ||
      rotate_angleZ <= -kRotateAngleLimit) {
    rotate_angleZ = kDefaultRotateAngle;
  }

  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      scaleX -= scale_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      scaleX += scale_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      scaleY += scale_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      scaleY -= scale_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
      scaleZ += scale_increment;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
      scaleZ -= scale_increment;
    }
  }

  if (scaleX > kScaleMax) {
    scaleX = kScaleMax;
  } else if (scaleX < kScaleMin) {
    scaleX = kScaleMin;
  }
  if (scaleY > kScaleMax) {
    scaleY = kScaleMax;
  } else if (scaleY < kScaleMin) {
    scaleY = kScaleMin;
  }
  if (scaleZ > kScaleMax) {
    scaleZ = kScaleMax;
  } else if (scaleZ < kScaleMin) {
    scaleZ = kScaleMin;
  }
}

void resize_window(GLFWwindow *window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);

  GLdouble aspect_ratio = (GLdouble)window_width / window_height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (orthographic_view) {
    glfwSetWindowTitle(window, "Cubo ortogonal");
    GLdouble left = -kOrthographicLimitX;
    GLdouble right = kOrthographicLimitX;
    GLdouble bottom = -kOrthographicLimitY;
    GLdouble top = kOrthographicLimitY;
    GLdouble near = -kOrthographicLimitZ;
    GLdouble far = kOrthographicLimitZ;
    if (window_width > window_height) {
      glOrtho((left * aspect_ratio), (right * aspect_ratio), bottom, top, near,
              far);
    } else {
      glOrtho(left, right, (bottom / aspect_ratio), (top / aspect_ratio), near,
              far);
    }
  } else {
    glfwSetWindowTitle(window, "Cubo em perspectiva");
    GLdouble field_of_view = kPerspectiveFieldOfViewAngle;
    GLdouble near = kPerspectiveNearZ;
    GLdouble far = kPerspectiveFarZ;
    gluPerspective(field_of_view, aspect_ratio, near, far);
  }
}

void draw() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLfloat perspectiveTranslateZ =
      orthographic_view ? 0.0f : -kPerspectiveTranslateZ;
  glTranslatef(translate, translateY, perspectiveTranslateZ + translateZ);
  glRotatef(rotate_angle, 1, 0, 0);
  glRotatef(rotate_angleY, 0, 1, 0);
  glRotatef(rotate_angleZ, 0, 0, 1);
  glScalef(scaleX, scaleY, scaleZ);

  glBegin(GL_QUADS);
  {
    // Face frontal (Z positivo).
    glColor3ub(255, 255, 255);
    glVertex3f(-1, -1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);

    // Face traseira (Z negativo).
    glColor3ub(255, 0, 0);
    glVertex3f(1, -1, -1);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);

    // Face esquerda (X negativo).
    glColor3ub(0, 255, 0);
    glVertex3f(-1, -1, -1);
    glVertex3f(-1, -1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1, 1, -1);

    // Face direita (X positivo).
    glColor3ub(0, 0, 255);
    glVertex3f(1, -1, 1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);

    // Face superior (Y positivo).
    glColor3ub(255, 255, 0);
    glVertex3f(-1, 1, -1);
    glVertex3f(1, 1, -1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);

    // Face inferior (Y negativo).
    glColor3ub(255, 0, 255);
    glVertex3f(-1, -1, -1);
    glVertex3f(1, -1, -1);
    glVertex3f(1, -1, 1);
    glVertex3f(-1, -1, 1);
  }
  glEnd();
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Falha ao inicializar GLFW" << std::endl;
    return EXIT_FAILURE;
  }

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  GLFWwindow *window =
      glfwCreateWindow(kWindowWidth, kWindowHeight, "", NULL, NULL);
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

  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    keyboard_read(window);
    resize_window(window);
    draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}

/*
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⡤⠤⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⡾⠉⠀⠀⠀⠀⠈⠙⠲⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⠋⠀⠀⠀⣀⠀⠀⣠⠀⢀⠘⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⠛⠁⠀⠀⠀⠙⣳⣶⣿⣫⣤⣼⣖⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⠏⠀⠢⢄⡴⣣⣾⠋⠻⣿⡾⣿⣿⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⢿⣴⡏⡴⠋⣴⡿⠻⠀⠀⢠⣴⡯⣹⢸⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⡤⠶⠒⠒⠲⠦⣄⣀⣀⣀⡽⠻⣧⣸⡿⣇⠀⢰⣾⣯⡿⢨⠏⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⠾⠃⠀⠀⠀⠀⠀⠀⠀⠑⠦⣌⠛⠦⣆⠋⠁⠘⢷⣄⠺⢿⣷⣻⣾⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⢀⡴⠞⠋⠉⣩⣉⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⣦⡈⠓⢿⡮⣴⣼⢿⣿⢻⣄⠙⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⣀⡤⠴⠤⣼⢛⡁⣠⣴⠟⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢻⣆⠀⠙⢦⡀⢪⡉⠉⠛⣿⠲⢤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⢠⡞⠉⠀⠀⠀⠀⠀⠹⣟⡁⠀⠀⠀⠀⠀⢀⡴⣲⣲⣦⠀⠀⠀⠀⠀⠀⠀⠀⢹⡆⠀⠀⣿⣶⣿⣶⣲⣃⠀⢸⠙⣦⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⡴⠟⠀⠀⠀⠀⠀⣠⣞⣻⣫⠷⡶⡲⡖⢲⣶⣟⣰⣿⡋⠙⣷⣦⣄⠀⢠⠴⠲⣄⠈⡇⢸⣷⣌⡻⣿⣿⡿⢁⣼⡎⡇⠰⣌⠹⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⢷⣿⣶⠦⠤⠤⢤⡿⠓⡆⡿⡇⢹⡀⢳⠀⣇⢻⡿⢮⢷⢀⡀⠈⠉⠉⠉⠀⢀⠛⠦⣿⢸⣿⣿⣿⣿⣿⣿⣿⣿⣷⡇⠃⠘⣧⠹⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠈⠻⣷⣀⠀⣈⣿⣞⣡⡷⢷⣿⣾⣿⣶⣏⣾⣹⢈⣿⣴⣉⣉⠒⠒⠒⠉⠉⠙⠲⢤⣉⠛⠻⠯⣿⣿⣯⡙⢿⡏⣿⠀⠀⢸⠀⢹⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠉⠙⠛⠛⠛⠛⠛⠛⠛⢫⣟⣿⣿⠿⢥⣾⠏⢷⣲⣢⣒⣛⢭⡀⠒⠒⠢⠤⠬⢭⣒⡦⣄⡈⠉⠉⠓⠓⠿⠤⣤⣏⣐⣾⠁⠀⠀⠀⠀⠀⢀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠉⠀⠀⠀⣏⣀⣴⠒⠚⡟⠽⡗⠊⢳⠒⠛⠉⠉⠉⢉⣉⣉⣉⠉⠙⠒⠒⠀⠤⠤⠤⣌⣉⡉⠛⠒⠒⠦⠴⢿⡋⣿⢹⣲⣤⡀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣦⡀⠀⠘⣦⣷⣄⡴⠚⢛⣿⣿⡋⠉⠉⠉⣩⠏⠉⠹⣿⠛⢲⣶⠶⠤⠤⢤⣈⣉⣑⣒⡶⠞⣟⣛⣲⣇⣇⣟⠒⠒⠦⢤⣀⡀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⣴⣾⠟⣻⠦⣤⣴⣿⡟⠋⠉⠉⠉⠙⢹⡗⠒⠒⣿⡇⣸⢧⡀⠀⢀⡼⠀⠀⠈⡿⠀⠘⢻⡿⢿⣿⠟⠥⠭⠭⣽⣒⣲⣭⠷
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⣿⣱⣾⠿⠛⣡⠖⢻⣧⣀⣀⣀⣀⣀⣸⠇⠀⢀⣿⣷⠟⣷⡽⠷⠿⣅⡤⡖⣿⣡⣤⣤⣼⢀⣀⡼⠁⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⠃⣀⡀⡇⠀⠈⢿⠉⠀⠀⠈⠉⢹⡟⠒⣞⣽⠟⢰⡏⠀⠀⢀⡟⡉⢀⣽⣷⣧⣼⡿⠋⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⢣⣇⣻⣿⣀⣀⣸⣆⣀⣀⣀⡀⣸⠃⠀⣸⣿⣀⡲⣷⣄⣀⣀⣷⣾⣿⣿⣿⣿⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⣿⣿⠿⠟⠛⠛⠛⠛⠛⡓⠛⠿⠿⠿⣽⣖⣷⣧⠙⢿⡛⠯⠍⠭⠵⠛⣛⡾⠛⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣶⣾⣿⣿⣶⣶⣶⣤⣄⣀⡀⠀⠀⠂⣹⡏⠀⠈⠻⠷⠤⠤⠴⠞⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⠟⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠛⠚⠛⠛⠛⠛⠛⠿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
Hemann*/
