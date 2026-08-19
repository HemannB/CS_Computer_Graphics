/*
 * *********************************************************************
 * File: square.cpp
 * Author: Bruno Hemann
 * Date: 12/08/2026
 * Description: Quadrado 2D com projeções, translação, rotação e escala.
 * *********************************************************************
 */

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <iostream>

// Configurações das projeções ortográfica e perspectiva
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
const GLfloat kPerspectiveTranslateZ = 12;

// Estado e limites da translação nos eixos X, Y e Z
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

// Estado e limites da rotação nos eixos X, Y e Z
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

// Estado e limites da escala aplicada nos eixos X, Y e Z
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

/*! @brief Função para verificar se uma combinação de teclas está sendo
 * pressionada.
 * @param window Ponteiro para a janela GLFW.
 * @param modifierKey Tecla modificadora (ex: GLFW_KEY_T, GLFW_KEY_R,
 * GLFW_KEY_E).
 * @param mainKey Tecla principal (ex: GLFW_KEY_LEFT, GLFW_KEY_RIGHT,
 * GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_PAGE_UP, GLFW_KEY_PAGE_DOWN).
 */
bool combinationKeys(GLFWwindow *window, int modifierKey, int mainKey);

/*! @brief Converte duas direções opostas em um valor entre -1 e 1. */
int direction(bool positive, bool negative);

/*! @brief Atualiza um valor, mantendo-o dentro dos limites informados. */
void updateLimited(GLfloat &value, bool increase, bool decrease,
                   GLfloat increment, GLfloat minimum, GLfloat maximum);

/*! @brief Atualiza um ângulo e o normaliza ao completar uma volta. */
void updateRotation(GLfloat &angle, bool positive, bool negative);

/*! @brief Atualiza os valores de translação, rotação e escala. */
void updateTransformations();

void keyboard_read(GLFWwindow *window) {
  // Controles gerais da aplicação.
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

  // Seleção do tipo de projeção.
  if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
    orthographic_view = true;
    perspective_view = false;
  }

  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    orthographic_view = false;
    perspective_view = true;
  }

  // Combinações para translação nos três eixos.
  translatingL = (combinationKeys(window, GLFW_KEY_T, GLFW_KEY_LEFT));
  translatingR = (combinationKeys(window, GLFW_KEY_T, GLFW_KEY_RIGHT));
  translatingU = (combinationKeys(window, GLFW_KEY_T, GLFW_KEY_UP));
  translatingD = (combinationKeys(window, GLFW_KEY_T, GLFW_KEY_DOWN));
  translatingNear = (combinationKeys(window, GLFW_KEY_T, GLFW_KEY_PAGE_DOWN));
  translatingFar = (combinationKeys(window, GLFW_KEY_T, GLFW_KEY_PAGE_UP));

  // Combinações para rotação nos três eixos.
  rotatingU = (combinationKeys(window, GLFW_KEY_R, GLFW_KEY_UP));
  rotatingD = (combinationKeys(window, GLFW_KEY_R, GLFW_KEY_DOWN));
  rotatingL = (combinationKeys(window, GLFW_KEY_R, GLFW_KEY_LEFT));
  rotatingR = (combinationKeys(window, GLFW_KEY_R, GLFW_KEY_RIGHT));
  rotatingAntiClockwise =
      (combinationKeys(window, GLFW_KEY_R, GLFW_KEY_PAGE_UP));
  rotatingClockwise = (combinationKeys(window, GLFW_KEY_R, GLFW_KEY_PAGE_DOWN));

  // Combinações para escala nos três eixos.
  scalingL = (combinationKeys(window, GLFW_KEY_E, GLFW_KEY_LEFT));
  scalingR = (combinationKeys(window, GLFW_KEY_E, GLFW_KEY_RIGHT));
  scalingU = (combinationKeys(window, GLFW_KEY_E, GLFW_KEY_UP));
  scalingD = (combinationKeys(window, GLFW_KEY_E, GLFW_KEY_DOWN));
  scalingNear = (combinationKeys(window, GLFW_KEY_E, GLFW_KEY_PAGE_DOWN));
  scalingFar = (combinationKeys(window, GLFW_KEY_E, GLFW_KEY_PAGE_UP));
}

// Ajuste da janela e configuração das projeções
void resize_window(GLFWwindow *window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);

  GLdouble aspect_ratio = (GLdouble)window_width / window_height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (orthographic_view) {
    glfwSetWindowTitle(window, "Quadrado ortogonal");
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
    glfwSetWindowTitle(window, "Quadrado em perspectiva");
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
  glRotatef(rotate_angle, 1, 0, 0);
  glRotatef(rotate_angleY, 0, 1, 0);
  glRotatef(rotate_angleZ, 0, 0, 1);
  glScalef(scaleX, scaleY, scaleZ);

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
  // Inicializa a GLFW antes de utilizar seus recursos.
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

  // Carrega as funções do OpenGL para o contexto criado.
  if (glewInit() != GLEW_OK) {
    std::cerr << "Falha ao inicializar GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  glEnable(GL_DEPTH_TEST);

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    keyboard_read(window);
    updateTransformations();
    resize_window(window);
    draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}

bool combinationKeys(GLFWwindow *window, int modifierKey, int mainKey) {
  return (glfwGetKey(window, modifierKey) == GLFW_PRESS &&
          glfwGetKey(window, mainKey) == GLFW_PRESS);
}

int direction(bool positive, bool negative) {
  return static_cast<int>(positive) - static_cast<int>(negative);
}

void updateLimited(GLfloat &value, bool increase, bool decrease,
                   GLfloat increment, GLfloat minimum, GLfloat maximum) {
  value = std::clamp(value + direction(increase, decrease) * increment, minimum,
                     maximum);
}

void updateRotation(GLfloat &angle, bool positive, bool negative) {
  angle =
      std::fmod(angle + direction(positive, negative) * rotate_angle_increment,
                kRotateAngleLimit);
}

void updateTransformations() {
  // Translação nos eixos X, Y e Z.
  updateLimited(translate, translatingR, translatingL, translate_increment,
                -kTranslateLimit, kTranslateLimit);
  updateLimited(translateY, translatingU, translatingD, translate_increment,
                -kTranslateLimit, kTranslateLimit);
  updateLimited(translateZ, translatingNear, translatingFar,
                translate_increment, -kTranslateLimit, kTranslateLimit);

  // Rotação nos eixos X, Y e Z.
  updateRotation(rotate_angle, rotatingU, rotatingD);
  updateRotation(rotate_angleY, rotatingL, rotatingR);
  updateRotation(rotate_angleZ, rotatingAntiClockwise, rotatingClockwise);

  // Escala nos eixos X, Y e Z.
  updateLimited(scaleX, scalingR, scalingL, scale_increment, kScaleMin,
                kScaleMax);
  updateLimited(scaleY, scalingU, scalingD, scale_increment, kScaleMin,
                kScaleMax);
  updateLimited(scaleZ, scalingNear, scalingFar, scale_increment, kScaleMin,
                kScaleMax);
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
