/*
 * *********************************************************************
 * File: cubeWithHudAndTextureAndMoviment.cpp
 * Author: Bruno Hemann
 * Date: 09/09/2026
 * Description: Descrição do arquivo
 * *********************************************************************
 */

#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>

#include "font.h"
#include "stb_image.h"
const int kWindowWidth = 800;
const int kWindowHeight = 800;

const GLfloat kCameraSpeed = 0.1;
const GLfloat kMouseSensitivity = 0.1;

const GLfloat kDefaultCameraEyePositionX = 0;
const GLfloat kDefaultCameraEyePositionY = 0;
const GLfloat kDefaultCameraEyePositionZ = 0;
GLfloat camera_eye_x = kDefaultCameraEyePositionX;
GLfloat camera_eye_y = kDefaultCameraEyePositionY;
GLfloat camera_eye_z = kDefaultCameraEyePositionZ;

const GLfloat kDefaultCameraDirectionX = 0;
const GLfloat kDefaultCameraDirectionY = 0;
const GLfloat kDefaultCameraDirectionZ = -1;
GLfloat camera_direction_x = kDefaultCameraDirectionX;
GLfloat camera_direction_y = kDefaultCameraDirectionY;
GLfloat camera_direction_z = kDefaultCameraDirectionZ;

const GLfloat kCameraUpX = 0;
const GLfloat kCameraUpY = 1;
const GLfloat kCameraUpZ = 0;

GLfloat camera_right_x = -1.0f;
GLfloat camera_right_y = 0.0f;
GLfloat camera_right_z = 0.0f;

const GLfloat kDefaultCameraYaw = -90;
GLfloat camera_yaw = kDefaultCameraYaw;

const GLfloat kCameraPitchLimit = 90;
const GLfloat kDefaultCameraPitch = 0;
GLfloat camera_pitch = kDefaultCameraPitch;

const GLfloat kPerspectiveFieldOfViewAngle = 45;
const GLfloat kPerspectiveNearZ = 0.1f;
const GLfloat kPerspectiveFarZ = 100;
const GLfloat kPerspectiveTranslateZ = 10;

bool reset_pressed = false;

bool translating = false;
const GLfloat kTranslateLimit = 3;
const GLfloat kDefaultTranslate = 0;
GLfloat translate_increment = 0.1f;
GLfloat translate = kDefaultTranslate;
GLfloat translateY = kDefaultTranslate;
GLfloat translateZ = kDefaultTranslate;

bool rotating = false;
const GLfloat kRotateAngleLimit = 360;
const GLfloat kDefaultRotateAngle = 0;
GLfloat rotate_angle_increment = 1.0f;
GLfloat rotate_angle = kDefaultRotateAngle;
GLfloat rotate_angleY = kDefaultRotateAngle;
GLfloat rotate_angleZ = kDefaultRotateAngle;

GLuint red_ball_texture_id;
GLuint green_ball_texture_id;
GLuint blue_ball_texture_id;
GLuint grass_texture_id;

bool scaling = false;
const GLfloat kScaleMin = 0.1f;
const GLfloat kScaleMax = 5.0f;
const GLfloat kDefaultScale = 1.0f;
GLfloat scale_increment = 0.1f;
GLfloat scaleX = kDefaultScale;
GLfloat scaleY = kDefaultScale;
GLfloat scaleZ = kDefaultScale;

const GLfloat kHudHeight = 300;
const GLfloat kHudWidth = 300;
std::ostringstream hud_text;

void load_texture(GLuint &texture_id, const std::string &filepath) {
  int texture_width, texture_height, texture_channels;
  uint8_t *texture_data = stbi_load(filepath.c_str(), &texture_width,
                                    &texture_height, &texture_channels, 0);

  if (!texture_data) {
    std::cerr << "Erro ao carregar a textura: " << filepath << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  GLenum texture_format = ((texture_channels == 4) ? GL_RGBA : GL_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, texture_format, texture_width, texture_height,
               0, texture_format, GL_UNSIGNED_BYTE, texture_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(texture_data);
}

static void mouse_read(GLFWwindow *window, double mouse_x, double mouse_y) {
  static bool mouse_initialized = false;
  static GLfloat last_mouse_x = 0, last_mouse_y = 0;
  if (!mouse_initialized) {
    last_mouse_x = mouse_x;
    last_mouse_y = mouse_y;
    mouse_initialized = true;
  }

  GLfloat mouse_delta_x = (mouse_x - last_mouse_x);
  GLfloat mouse_delta_y = (last_mouse_y - mouse_y);
  last_mouse_x = mouse_x;
  last_mouse_y = mouse_y;

  camera_pitch += (mouse_delta_y * kMouseSensitivity);
  if (camera_pitch > kCameraPitchLimit) {
    camera_pitch = kCameraPitchLimit;
  }
  if (camera_pitch < -kCameraPitchLimit) {
    camera_pitch = -kCameraPitchLimit;
  }
  camera_yaw += (mouse_delta_x * kMouseSensitivity);

  GLfloat camera_pitch_radians = camera_pitch * (3.1415 / 180);
  GLfloat camera_yaw_radians = camera_yaw * (3.1415 / 180);
  camera_direction_x = cosf(camera_yaw_radians) * cosf(camera_pitch_radians);
  camera_direction_y = sinf(camera_pitch_radians);
  camera_direction_z = sinf(camera_yaw_radians) * cosf(camera_pitch_radians);

  camera_right_x =
      camera_direction_z * kCameraUpY - camera_direction_y * kCameraUpZ;
  camera_right_y =
      camera_direction_x * kCameraUpZ - camera_direction_z * kCameraUpX;
  camera_right_z =
      camera_direction_y * kCameraUpX - camera_direction_x * kCameraUpY;
}

void keyboard_read(GLFWwindow *window) {
  translating = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
  rotating = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
  scaling = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera_eye_x += camera_direction_x * kCameraSpeed;
    camera_eye_y += camera_direction_y * kCameraSpeed;
    camera_eye_z += camera_direction_z * kCameraSpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera_eye_x -= camera_direction_x * kCameraSpeed;
    camera_eye_y -= camera_direction_y * kCameraSpeed;
    camera_eye_z -= camera_direction_z * kCameraSpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera_eye_x += camera_right_x * kCameraSpeed;
    camera_eye_y += camera_right_y * kCameraSpeed;
    camera_eye_z += camera_right_z * kCameraSpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera_eye_x -= camera_right_x * kCameraSpeed;
    camera_eye_y -= camera_right_y * kCameraSpeed;
    camera_eye_z -= camera_right_z * kCameraSpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    reset_pressed = true;
    translate = kDefaultTranslate;
    translateY = kDefaultTranslate;
    translateZ = kDefaultTranslate;
    rotate_angle = kDefaultRotateAngle;
    rotate_angleY = kDefaultRotateAngle;
    rotate_angleZ = kDefaultRotateAngle;
    scaleX = kDefaultScale;
    scaleY = kDefaultScale;
    scaleZ = kDefaultScale;

    camera_eye_x = kDefaultCameraEyePositionX;
    camera_eye_y = kDefaultCameraEyePositionY;
    camera_eye_z = kDefaultCameraEyePositionZ;

    camera_direction_x = kDefaultCameraDirectionX;
    camera_direction_y = kDefaultCameraDirectionY;
    camera_direction_z = kDefaultCameraDirectionZ;

    camera_right_x = -1.0f;
    camera_right_y = 0.0f;
    camera_right_z = 0.0f;

    camera_yaw = kDefaultCameraYaw;
    camera_pitch = kDefaultCameraPitch;
  }

  if (translating) {
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

  if (rotating) {
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

  if (rotate_angle >= kRotateAngleLimit || rotate_angle <= -kRotateAngleLimit) {
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

  if (scaling) {
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
}
/*! @brief Atualiza o texto do HUD dado o estado atual... */
void update_hud() {
  hud_text.str("");

  if (translating) {
    hud_text << "Transladando (" << translate << ", " << translateY << ", "
             << translateZ << ")";
  } else if (rotating) {
    hud_text << "Rotacionando (" << rotate_angle << ", " << rotate_angleY
             << ", " << rotate_angleZ << ")";
  } else if (scaling) {
    hud_text << "Escalando (" << scaleX << ", " << scaleY << ", " << scaleZ
             << ")";
  } else if (reset_pressed) {
    hud_text << "Redefinido!";
    reset_pressed = false;
  }
}

void draw(GLFWwindow *window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  GLdouble aspect_ratio = (GLdouble)window_width / window_height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(kPerspectiveFieldOfViewAngle, aspect_ratio, kPerspectiveNearZ,
                 kPerspectiveFarZ);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLfloat camera_center_x = camera_eye_x + camera_direction_x;
  GLfloat camera_center_y = camera_eye_y + camera_direction_y;
  GLfloat camera_center_z = camera_eye_z + camera_direction_z;

  gluLookAt(camera_eye_x, camera_eye_y, camera_eye_z, camera_center_x,
            camera_center_y, camera_center_z, kCameraUpX, kCameraUpY,
            kCameraUpZ);

  glTranslatef(translate, translateY, -kPerspectiveTranslateZ + translateZ);
  glRotatef(rotate_angle, 1, 0, 0);
  glRotatef(rotate_angleY, 0, 1, 0);
  glRotatef(rotate_angleZ, 0, 0, 1);
  glScalef(scaleX, scaleY, scaleZ);

  glColor3ub(255, 255, 255);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, grass_texture_id);

  glBegin(GL_QUADS);
  {
    // Face frontal (Z positivo)
    glVertex3f(-1, -1, 1);
    glVertex3f(1, -1, 1);
    glVertex3f(1, 1, 1);
    glVertex3f(-1, 1, 1);

    // Face traseira (Z negativo).
    glTexCoord2f(0, 0);
    glVertex3f(1, -1, -1);
    glTexCoord2f(1, 0);
    glVertex3f(-1, -1, -1);
    glTexCoord2f(1, 1);
    glVertex3f(-1, 1, -1);
    glTexCoord2f(0, 1);
    glVertex3f(1, 1, -1);

    // Face esquerda (X negativo).
    glTexCoord2f(0, 0);
    glVertex3f(-1, -1, -1);
    glTexCoord2f(1, 0);
    glVertex3f(-1, -1, 1);
    glTexCoord2f(1, 1);
    glVertex3f(-1, 1, 1);
    glTexCoord2f(0, 1);
    glVertex3f(-1, 1, -1);

    // Face direita (X positivo).
    glTexCoord2f(0, 0);
    glVertex3f(1, -1, 1);
    glTexCoord2f(1, 0);
    glVertex3f(1, -1, -1);
    glTexCoord2f(1, 1);
    glVertex3f(1, 1, -1);
    glTexCoord2f(0, 1);
    glVertex3f(1, 1, 1);

    // Face superior (Y positivo).
    glTexCoord2f(0, 0);
    glVertex3f(-1, 1, -1);
    glTexCoord2f(1, 0);
    glVertex3f(1, 1, -1);
    glTexCoord2f(1, 1);
    glVertex3f(1, 1, 1);
    glTexCoord2f(0, 1);
    glVertex3f(-1, 1, 1);

    // Face inferior (Y negativo).
    glTexCoord2f(0, 0);
    glVertex3f(-1, -1, -1);
    glTexCoord2f(1, 0);
    glVertex3f(1, -1, -1);
    glTexCoord2f(1, 1);
    glVertex3f(1, -1, 1);
    glTexCoord2f(0, 1);
    glVertex3f(-1, -1, 1);
  }
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

void draw_hud(GLFWwindow *window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  GLdouble aspect_ratio = (GLdouble)window_width / window_height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  GLdouble left = 0;
  GLdouble right = kHudWidth;
  GLdouble bottom = 0;
  GLdouble top = kHudHeight;
  if (window_width > window_height) {
    gluOrtho2D((left * aspect_ratio), (right * aspect_ratio), bottom, top);
  } else {
    gluOrtho2D(left, right, (bottom / aspect_ratio), (top / aspect_ratio));
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  {
    glColor3ub(255, 255, 255);
    draw_text(5, 5, hud_text.str());

    glEnable(GL_TEXTURE_2D);

    {
      if (translating) {
        glBindTexture(GL_TEXTURE_2D, red_ball_texture_id);
        glBegin(GL_QUADS);
        {
          glTexCoord2f(0, 0);
          glVertex2f(5, 15);
          glTexCoord2f(1, 0);
          glVertex2f(15, 15);
          glTexCoord2f(1, 1);
          glVertex2f(15, 25);
          glTexCoord2f(0, 1);
          glVertex2f(5, 25);
        }
        glEnd();
      }

      if (rotating) {
        glBindTexture(GL_TEXTURE_2D, green_ball_texture_id);
        glBegin(GL_QUADS);
        {
          glTexCoord2f(0, 0);
          glVertex2f(15, 15);
          glTexCoord2f(1, 0);
          glVertex2f(25, 15);
          glTexCoord2f(1, 1);
          glVertex2f(25, 25);
          glTexCoord2f(0, 1);
          glVertex2f(15, 25);
        }
        glEnd();
      }

      if (scaling) {
        glBindTexture(GL_TEXTURE_2D, blue_ball_texture_id);
        glBegin(GL_QUADS);
        {
          glTexCoord2f(0, 0);
          glVertex2f(25, 15);
          glTexCoord2f(1, 0);
          glVertex2f(35, 15);
          glTexCoord2f(1, 1);
          glVertex2f(35, 25);
          glTexCoord2f(0, 1);
          glVertex2f(25, 25);
        }
        glEnd();
      }
    }
    glDisable(GL_TEXTURE_2D);
  }
  glEnable(GL_DEPTH_TEST);
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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_read);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Falha ao inicializar GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  glEnable(GL_DEPTH_TEST);

  load_texture(red_ball_texture_id, "textures/red_ball.png");
  load_texture(green_ball_texture_id, "textures/green_ball.png");
  load_texture(blue_ball_texture_id, "textures/blue_ball.png");
  load_texture(grass_texture_id, "textures/lava.png");

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    keyboard_read(window);
    update_hud();
    resize_window(window);
    draw(window);
    draw_hud(window);

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
