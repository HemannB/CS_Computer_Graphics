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
const GLfloat kOrthographicLimitX = 5;
const GLfloat kOrthographicLimitY = 5;
const GLfloat kOrthographicLimitZ = 20;

bool perspective_view = false;
const GLfloat kPerspectiveFieldOfViewAngle = 45;
const GLfloat kPerspectiveNearZ = 0.1f;
const GLfloat kPerspectiveFarZ = 100;
const GLfloat kPerspectiveTranslateZ = 10;

bool translatingL = false;
bool translatingR = false;
bool translatingU = false;
bool translatingD = false;
bool translatingNear = false;
bool translatingFar = false;
const GLfloat kTranslateLimit = 3;
const GLfloat kDefaultTranslate = 0;
GLfloat translate_increment = 0.1f;
GLfloat translate = kDefaultTranslate;
GLfloat translateY = kDefaultTranslate;
GLfloat translateZ = kDefaultTranslate;

bool rotatingU = false;
bool rotatingD = false;
bool rotatingL = false;
bool rotatingR = false;
bool rotatingAntiClockwise = false;
bool rotatingClockwise = false;
const GLfloat kRotateAngleLimit = 360;
const GLfloat kDefaultRotateAngle = 0;
GLfloat rotate_angle_increment = 1.0f;
GLfloat rotate_angle = kDefaultRotateAngle;
GLfloat rotate_angleY = kDefaultRotateAngle;
GLfloat rotate_angleZ = kDefaultRotateAngle;

bool scalingL = false;
bool scalingR = false;
bool scalingU = false;
bool scalingD = false;
bool scalingNear = false;
bool scalingFar = false;
const GLfloat kScaleMin = 0.1f;
const GLfloat kScaleMax = 10.0f;
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
    perspective_view = false;
  }

  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    orthographic_view = false;
    perspective_view = true;
  }

  bool translateLeftPressed = (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS &&
                               glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
  bool translateRightPressed =
      (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS &&
       glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
  bool translateUpPressed = (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS &&
                             glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
  bool translateDownPressed = (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS &&
                               glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
  bool translateNearPressed =
      (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS &&
       glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS);
  bool translateFarPressed =
      (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS &&
       glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS);

  translatingL = translateLeftPressed;
  translatingR = translateRightPressed;
  translatingU = translateUpPressed;
  translatingD = translateDownPressed;
  translatingNear = translateNearPressed;
  translatingFar = translateFarPressed;

  bool rotateUpPressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&
                          glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
  bool rotateDownPressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&
                            glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
  bool rotateLeftPressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&
                            glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
  bool rotateRightPressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&
                             glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
  bool rotateAntiClockwisePressed =
      (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&
       glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS);
  bool rotateClockwisePressed =
      (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&
       glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS);

  rotatingU = rotateUpPressed;
  rotatingD = rotateDownPressed;
  rotatingL = rotateLeftPressed;
  rotatingR = rotateRightPressed;
  rotatingAntiClockwise = rotateAntiClockwisePressed;
  rotatingClockwise = rotateClockwisePressed;

  bool scaleLeftPressed = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS &&
                           glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
  bool scaleRightPressed = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS &&
                            glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
  bool scaleUpPressed = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS &&
                         glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
  bool scaleDownPressed = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS &&
                           glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
  bool scaleNearPressed =
      (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS &&
       glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS);
  bool scaleFarPressed = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS &&
                          glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS);

  scalingL = scaleLeftPressed;
  scalingR = scaleRightPressed;
  scalingU = scaleUpPressed;
  scalingD = scaleDownPressed;
  scalingNear = scaleNearPressed;
  scalingFar = scaleFarPressed;
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
  }
  if (perspective_view) {
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
      perspective_view ? -kPerspectiveTranslateZ : 0.0f;
  glTranslatef(translate, translateY, perspectiveTranslateZ + translateZ);

  if (translatingL) {
    translate -= translate_increment;
    if (translate < -kTranslateLimit) {
      translate = -kTranslateLimit;
    }
  }
  if (translatingR) {
    translate += translate_increment;
    if (translate > kTranslateLimit) {
      translate = kTranslateLimit;
    }
  }
  if (translatingU) {
    translateY += translate_increment;
    if (translateY > kTranslateLimit) {
      translateY = kTranslateLimit;
    }
  }
  if (translatingD) {
    translateY -= translate_increment;
    if (translateY < -kTranslateLimit) {
      translateY = -kTranslateLimit;
    }
  }
  if (translatingNear) {
    translateZ += translate_increment;
    if (translateZ > kTranslateLimit) {
      translateZ = kTranslateLimit;
    }
  }
  if (translatingFar) {
    translateZ -= translate_increment;
    if (translateZ < -kTranslateLimit) {
      translateZ = -kTranslateLimit;
    }
  }

  glRotatef(rotate_angle, 1, 0, 0);
  if (rotatingU) {
    rotate_angle += rotate_angle_increment;
    if (rotate_angle >= kRotateAngleLimit) {
      rotate_angle = kDefaultRotateAngle;
    }
  }
  if (rotatingD) {
    rotate_angle -= rotate_angle_increment;
    if (rotate_angle <= -kRotateAngleLimit) {
      rotate_angle = kDefaultRotateAngle;
    }
  }

  glRotatef(rotate_angleY, 0, 1, 0);
  if (rotatingL) {
    rotate_angleY += rotate_angle_increment;
    if (rotate_angleY >= kRotateAngleLimit) {
      rotate_angleY = kDefaultRotateAngle;
    }
  }
  if (rotatingR) {
    rotate_angleY -= rotate_angle_increment;
    if (rotate_angleY <= -kRotateAngleLimit) {
      rotate_angleY = kDefaultRotateAngle;
    }
  }

  glRotatef(rotate_angleZ, 0, 0, 1);
  if (rotatingAntiClockwise) {
    rotate_angleZ += rotate_angle_increment;
    if (rotate_angleZ >= kRotateAngleLimit) {
      rotate_angleZ = kDefaultRotateAngle;
    }
  }
  if (rotatingClockwise) {
    rotate_angleZ -= rotate_angle_increment;
    if (rotate_angleZ <= -kRotateAngleLimit) {
      rotate_angleZ = kDefaultRotateAngle;
    }
  }

  glScalef(scaleX, scaleY, scaleZ);
  if (scalingL) {
    scaleX -= scale_increment;
    if (scaleX < kScaleMin) {
      scaleX = kScaleMin;
    }
  }
  if (scalingR) {
    scaleX += scale_increment;
    if (scaleX > kScaleMax) {
      scaleX = kScaleMax;
    }
  }
  if (scalingU) {
    scaleY += scale_increment;
    if (scaleY > kScaleMax) {
      scaleY = kScaleMax;
    }
  }
  if (scalingD) {
    scaleY -= scale_increment;
    if (scaleY < kScaleMin) {
      scaleY = kScaleMin;
    }
  }
  if (scalingNear) {
    scaleZ += scale_increment;
    if (scaleZ > kScaleMax) {
      scaleZ = kScaleMax;
    }
  }
  if (scalingFar) {
    scaleZ -= scale_increment;
    if (scaleZ < kScaleMin) {
      scaleZ = kScaleMin;
    }
  }

  glBegin(GL_QUADS);
  {
    glColor3ub(255, 255, 255);
    glVertex3f(-2, -2, -2);
    glVertex3f(2, -2, -2);
    glVertex3f(2, 2, -2);
    glVertex3f(-2, 2, -2);

    glColor3ub(255, 0, 0);
    glVertex3f(-2, -2, -2);
    glVertex3f(2, -2, -2);
    glVertex3f(2, 2, -2);
    glVertex3f(-2, 2, -2);

    glColor3ub(0, 255, 0);
    glVertex3f(-2, -2, -2);
    glVertex3f(2, -2, -2);
    glVertex3f(2, -2, 2);
    glVertex3f(-2, -2, 2);

    glColor3ub(0, 0, 255);
    glVertex3f(-2, 2, -2);
    glVertex3f(2, 2, -2);
    glVertex3f(2, 2, 2);
    glVertex3f(-2, 2, 2);
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
