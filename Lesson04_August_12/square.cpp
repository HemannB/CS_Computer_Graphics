/*
 * *********************************************************************
 * File: square.cpp
 * Author: Bruno Hemann
 * Date: 12/08/2026
 * Description: Quadrado 2D com projeções, translação, rotação e escala.
 * *********************************************************************
 */

// Bibliotecas gráficas: carregamento do OpenGL, projeções e gerenciamento da
// janela e de seus eventos.
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

// Biblioteca padrão usada para mensagens no terminal.
#include <iostream>

// -----------------------------------------------------------------------------
// Configurações das projeções ortográfica e perspectiva
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
// Estado e limites da translação nos eixos X, Y e Z
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
// Estado e limites da rotação nos eixos X, Y e Z
// -----------------------------------------------------------------------------

bool rotatingU = false;
bool rotatingD = false;
bool rotatingL = false;
bool rotatingR = false;
bool rotatingCounterClockwise = false;
bool rotatingClockwise = false;
const GLfloat kRotateAngleLimit = 360;
const GLfloat kDefaultRotateAngle = 0;
GLfloat rotate_angle_increment = 1.0f;
GLfloat rotate_angle = kDefaultRotateAngle;
GLfloat rotate_angleY = kDefaultRotateAngle;
GLfloat rotate_angleZ = kDefaultRotateAngle;

// -----------------------------------------------------------------------------
// Estado e limites da escala aplicada no eixo X
// -----------------------------------------------------------------------------

bool scaling = false;
const GLfloat kScaleLimit = 10;
const GLfloat kDefaultScale = 1;
GLfloat scale_increment = 0.1f;
GLfloat scale = kDefaultScale;

// -----------------------------------------------------------------------------
// Leitura do teclado
//
// ESC              Fecha a janela.
// ESPAÇO           Restaura todas as transformações.
// O / P            Alterna entre projeção ortográfica e perspectiva.
// T + setas        Translada nos eixos X e Y.
// T + Page Up/Down Translada no eixo Z.
// R + setas        Rotaciona nos eixos X e Y.
// R + Page Up/Down Rotaciona no eixo Z.
// E                Aplica escala no eixo X.
// -----------------------------------------------------------------------------

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
    scale = kDefaultScale;
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

  // Combinações para rotação nos três eixos.
  bool rotateUpPressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&
                          glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
  bool rotateDownPressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&
                            glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
  bool rotateLeftPressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&
                            glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
  bool rotateRightPressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&
                             glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
  bool rotateCounterClockwisePressed =
      (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&
       glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS);
  bool rotateClockwisePressed =
      (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS &&
       glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS);

  rotatingU = rotateUpPressed;
  rotatingD = rotateDownPressed;
  rotatingL = rotateLeftPressed;
  rotatingR = rotateRightPressed;
  rotatingCounterClockwise = rotateCounterClockwisePressed;
  rotatingClockwise = rotateClockwisePressed;

  // A escala permanece ativa enquanto a tecla E estiver pressionada.
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    scaling = true;
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
    scaling = false;
  }
}

// -----------------------------------------------------------------------------
// Projeção para uma janela fixa e quadrada
// -----------------------------------------------------------------------------

void configure_projection(GLFWwindow *window) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  // Projeção ortográfica.
  if (orthographic_view) {
    glfwSetWindowTitle(window, "Quadrado ortogonal");
    glOrtho(-kOrthographicLimitX, kOrthographicLimitX,
            -kOrthographicLimitY, kOrthographicLimitY,
            -kOrthographicLimitZ, kOrthographicLimitZ);
  }

  // A janela é quadrada, portanto sua proporção é fixa em 1.0.
  if (perspective_view) {
    glfwSetWindowTitle(window, "Quadrado em perspectiva");
    gluPerspective(kPerspectiveFieldOfViewAngle, 1.0, kPerspectiveNearZ,
                   kPerspectiveFarZ);
  }
}

// -----------------------------------------------------------------------------
// Transformações e desenho
//
// Prepara a matriz de modelo, atualiza as transformações conforme as teclas
// pressionadas e desenha o quadrado.
// -----------------------------------------------------------------------------

void draw() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Na perspectiva, desloca o objeto para a região visível da câmera.
  GLfloat perspectiveTranslateZ =
      perspective_view ? -kPerspectiveTranslateZ : 0.0f;
  glTranslatef(translate, translateY,
               perspectiveTranslateZ + translateZ);

  // Translação no eixo X, limitada ao intervalo configurado.
  if (translatingL) {
    std::cout << "Translating Left: " << translate << std::endl;
    if (translate > -kTranslateLimit) {
      translate -= translate_increment;
      if (translate < -kTranslateLimit) {
        translate = -kTranslateLimit;
      }
    }
  }
  if (translatingR) {
    if (translate < kTranslateLimit) {
      translate += translate_increment;
      if (translate > kTranslateLimit) {
        translate = kTranslateLimit;
      }
    }
  }

  // Translação no eixo Y, limitada ao intervalo configurado.
  if (translatingU) {
    if (translateY < kTranslateLimit) {
      translateY += translate_increment;
      if (translateY > kTranslateLimit) {
        translateY = kTranslateLimit;
      }
    }
  }
  if (translatingD) {
    if (translateY > -kTranslateLimit) {
      translateY -= translate_increment;
      if (translateY < -kTranslateLimit) {
        translateY = -kTranslateLimit;
      }
    }
  }

  // Translação no eixo Z: Near aproxima e Far afasta o objeto da câmera.
  if (translatingNear) {
    std::cout << "Translating Near: " << translateZ << std::endl;
    if (translateZ < kTranslateLimit) {
      translateZ += translate_increment;
      if (translateZ > kTranslateLimit) {
        translateZ = kTranslateLimit;
      }
    }
  }
  if (translatingFar) {
    std::cout << "Translating Far: " << translateZ << std::endl;
    if (translateZ > -kTranslateLimit) {
      translateZ -= translate_increment;
      if (translateZ < -kTranslateLimit) {
        translateZ = -kTranslateLimit;
      }
    }
  }

  // Rotação no eixo X. Ao completar uma volta, o ângulo retorna a zero.
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

  // Rotação no eixo Y. Ao completar uma volta, o ângulo retorna a zero.
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

  // Rotação no eixo Z. Page Up gira no sentido anti-horário e Page Down no
  // sentido horário.
  glRotatef(rotate_angleZ, 0, 0, 1);
  if (rotatingCounterClockwise) {
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

  // Escala no eixo X, alternando a direção ao alcançar cada limite.
  glScalef(scale, 1, 1);
  if (scaling) {
    scale += scale_increment;
    if (scale >= kScaleLimit) {
      scale = kScaleLimit;
      scale_increment *= -1;
    } else if (scale <= -kScaleLimit) {
      scale = -kScaleLimit;
      scale_increment *= -1;
    }
  }

  // Geometria e cores dos quatro vértices do quadrado.
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

// -----------------------------------------------------------------------------
// Inicialização, janela e loop principal da aplicação
// -----------------------------------------------------------------------------

int main() {
  // Inicializa a GLFW antes de utilizar seus recursos.
  if (!glfwInit()) {
    std::cerr << "Falha ao inicializar GLFW" << std::endl;
    return EXIT_FAILURE;
  }

  // Cria uma janela quadrada, pequena e com tamanho fixo.
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

  // Faz o OpenGL respeitar a profundidade dos objetos desenhados.
  glEnable(GL_DEPTH_TEST);

  // Loop principal: limpa, atualiza o estado, desenha e processa eventos.
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    keyboard_read(window);
    configure_projection(window);
    draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Libera a janela e encerra a GLFW antes de finalizar o programa.
  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}
