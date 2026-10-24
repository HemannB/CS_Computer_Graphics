/*
 * *********************************************************************
 * File: main.cpp
 * Author: Bruno Hemann
 * Date: 14/10/2026
 * Description: Cena 3D de uma batalha no deserto de Tatooine com
 *              iluminacao e controle do sol baseados no Exemplo 12.
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
#include <vector>

#include "glb_model.h"

const GLfloat kCameraSpeed = 0.1f;
const GLfloat kMouseSensitivity = 0.1;

const GLfloat kDefaultCameraEyePositionX = 0;
const GLfloat kDefaultCameraEyePositionY = 0;
const GLfloat kDefaultCameraEyePositionZ = 0;
GLfloat camera_eye_x = kDefaultCameraEyePositionX;
GLfloat camera_eye_y = kDefaultCameraEyePositionY;
GLfloat camera_eye_z = kDefaultCameraEyePositionZ;

const GLfloat kDefaultCameraDirectionX = 0;
const GLfloat kDefaultCameraDirectionY = 0.114937f;
const GLfloat kDefaultCameraDirectionZ = -0.993373f;
GLfloat camera_direction_x = kDefaultCameraDirectionX;
GLfloat camera_direction_y = kDefaultCameraDirectionY;
GLfloat camera_direction_z = kDefaultCameraDirectionZ;

const GLfloat kCameraUpX = 0;
const GLfloat kCameraUpY = 1;
const GLfloat kCameraUpZ = 0;

const GLfloat kDefaultCameraRightX = -0.993f;
const GLfloat kDefaultCameraRightY = 0;
const GLfloat kDefaultCameraRightZ = 0;
GLfloat camera_right_x = kDefaultCameraRightX;
GLfloat camera_right_y = 0;
GLfloat camera_right_z = kDefaultCameraRightZ;

const GLfloat kDefaultCameraYaw = -90;
GLfloat camera_yaw = kDefaultCameraYaw;

const GLfloat kCameraPitchLimit = 89;
const GLfloat kDefaultCameraPitch = 6.600f;
GLfloat camera_pitch = kDefaultCameraPitch;

const GLfloat kPerspectiveFieldOfViewAngle = 45;
const GLfloat kPerspectiveNearZ = 0.1;
const GLfloat kPerspectiveFarZ = 150;

GLuint skybox_texture_id;
GLuint sun_glow_texture_id;

GLuint death_star_model_display_list_id = 0;
GLuint at_st_model_display_list_id = 0;
GLuint battle_droid_model_display_list_id = 0;
GLuint sun_model_display_list_id = 0;
GLuint jedi_model_display_list_id = 0;
GLuint anakin_model_display_list_id = 0;
GLuint r2d2_model_display_list_id = 0;
GLuint canyon_model_display_list_id = 0;

GlbModel death_star_model;
GlbModel at_st_model;
GlbModel battle_droid_model;
GlbModel sun_model;
GlbModel jedi_model;
GlbModel anakin_model;
GlbModel r2d2_model;
GlbModel canyon_model;

const GLfloat kSkyColorDay[] = {0.46, 0.62, 0.82, 1};
const GLfloat kSkyColorNight[] = {0.0, 0.0, 0.2, 1};

const GLfloat kPrimarySunAmbient[] = {0.10, 0.08, 0.05, 1};
const GLfloat kPrimarySunDiffuse[] = {0.95, 0.78, 0.50, 1};
const GLfloat kPrimarySunSpecular[] = {0.95, 0.85, 0.65, 1};
const GLfloat kPrimarySunEmission[] = {1.0, 0.82, 0.50, 1};

const GLfloat kSecondarySunAmbient[] = {0.06, 0.06, 0.05, 1};
const GLfloat kSecondarySunDiffuse[] = {0.70, 0.72, 0.65, 1};
const GLfloat kSecondarySunSpecular[] = {0.80, 0.80, 0.70, 1};
const GLfloat kSecondarySunEmission[] = {1.0, 0.95, 0.75, 1};

const GLfloat kSunConstantAttenuation = 0.1;
const GLfloat kSunLinearAttenuation = 0.05;
const GLfloat kSunQuadraticAttenuation = 0;

const GLfloat kSunTranslateIncrement = 0.1;
const GLfloat kMinimumSunPositionY = 1;
const GLfloat kDefaultPrimarySunPositionX = -4.5f;
const GLfloat kDefaultPrimarySunPositionY = 28.3f;
const GLfloat kDefaultPrimarySunPositionZ = -58.9f;

GLfloat primary_sun_position[] = {kDefaultPrimarySunPositionX,
                                  kDefaultPrimarySunPositionY,
                                  kDefaultPrimarySunPositionZ, 1};

const GLfloat kOrbitingSunSpeed = 0.35f;
const GLfloat kOrbitingSunRadiusX = 9.0f;
const GLfloat kOrbitingSunRadiusY = 6.0f;
const GLfloat kOrbitingSunRadiusZ = 2.0f;
GLfloat secondary_sun_position[] = {
    kDefaultPrimarySunPositionX + kOrbitingSunRadiusX,
    kDefaultPrimarySunPositionY, kDefaultPrimarySunPositionZ, 1};

const GLfloat kSceneSpecular[] = {0.35, 0.32, 0.28, 1};
const GLfloat kSceneShininess = 16;

void create_model_display_list(GLuint &model_display_list_id,
                               const GlbModel &model) {
  model_display_list_id = glGenLists(1);
  glNewList(model_display_list_id, GL_COMPILE);
  {
    glEnable(GL_TEXTURE_2D);
    draw_model(model);
    glDisable(GL_TEXTURE_2D);
  }
  glEndList();
}

void update_orbiting_sun_position() {
  const GLfloat elapsed_time = static_cast<GLfloat>(glfwGetTime());
  const GLfloat orbit_angle = elapsed_time * kOrbitingSunSpeed;

  secondary_sun_position[0] =
      primary_sun_position[0] + cosf(orbit_angle) * kOrbitingSunRadiusX;
  secondary_sun_position[1] =
      primary_sun_position[1] + sinf(orbit_angle) * kOrbitingSunRadiusY;
  secondary_sun_position[2] =
      primary_sun_position[2] + sinf(orbit_angle) * kOrbitingSunRadiusZ;
}

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

  GLint previous_unpack_alignment;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &previous_unpack_alignment);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, texture_format, texture_width, texture_height,
               0, texture_format, GL_UNSIGNED_BYTE, texture_data);
  glPixelStorei(GL_UNPACK_ALIGNMENT, previous_unpack_alignment);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(texture_data);
}

void create_sun_glow_texture() {
  const GLint texture_size = 128;
  std::vector<uint8_t> texture_data(texture_size * texture_size * 4);

  for (GLint y = 0; y < texture_size; y++) {
    for (GLint x = 0; x < texture_size; x++) {
      const GLfloat normalized_x = (2.0f * (x + 0.5f) / texture_size) - 1.0f;
      const GLfloat normalized_y = (2.0f * (y + 0.5f) / texture_size) - 1.0f;
      const GLfloat distance =
          sqrtf(normalized_x * normalized_x + normalized_y * normalized_y);
      const GLfloat fade = ((distance < 1.0f) ? (1.0f - distance) : 0.0f);
      const GLfloat intensity = fade * fade;
      const size_t pixel = (y * texture_size + x) * 4;

      texture_data[pixel] = 255;                                            // R
      texture_data[pixel + 1] = static_cast<uint8_t>(180 + 75 * intensity); // G
      texture_data[pixel + 2] = static_cast<uint8_t>(40 + 180 * intensity); // B
      texture_data[pixel + 3] = static_cast<uint8_t>(210 * intensity);      // A
    }
  }

  glGenTextures(1, &sun_glow_texture_id);
  glBindTexture(GL_TEXTURE_2D, sun_glow_texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_size, texture_size, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, texture_data.data());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

static void mouse_read(GLFWwindow *window, double mouse_x, double mouse_y) {
  (void)window;
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
    camera_eye_x = kDefaultCameraEyePositionX;
    camera_eye_y = kDefaultCameraEyePositionY;
    camera_eye_z = kDefaultCameraEyePositionZ;
    camera_direction_x = kDefaultCameraDirectionX;
    camera_direction_y = kDefaultCameraDirectionY;
    camera_direction_z = kDefaultCameraDirectionZ;
    camera_right_x = kDefaultCameraRightX;
    camera_right_y = kDefaultCameraRightY;
    camera_right_z = kDefaultCameraRightZ;
    camera_yaw = kDefaultCameraYaw;
    camera_pitch = kDefaultCameraPitch;

    primary_sun_position[0] = kDefaultPrimarySunPositionX;
    primary_sun_position[1] = kDefaultPrimarySunPositionY;
    primary_sun_position[2] = kDefaultPrimarySunPositionZ;
  }

  if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      primary_sun_position[0] -= kSunTranslateIncrement;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      primary_sun_position[0] += kSunTranslateIncrement;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS &&
        primary_sun_position[1] > kMinimumSunPositionY) {
      primary_sun_position[1] -= kSunTranslateIncrement;
      if (primary_sun_position[1] < kMinimumSunPositionY) {
        primary_sun_position[1] = kMinimumSunPositionY;
      }
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      primary_sun_position[1] += kSunTranslateIncrement;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
      primary_sun_position[2] -= kSunTranslateIncrement;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
      primary_sun_position[2] += kSunTranslateIncrement;
    }
  }
}

void resize_window(GLFWwindow *window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);
}

void draw_at_st(GLfloat x, GLfloat y, GLfloat z, GLfloat rotation_y,
                GLfloat scale) {
  glPushMatrix();
  {
    glTranslatef(x, y, z);
    glRotatef(rotation_y, 0, 1, 0);
    glScalef(scale, scale, scale);
    glCallList(at_st_model_display_list_id);
  }
  glPopMatrix();
}

void draw_battle_droid(GLfloat x, GLfloat y, GLfloat z, GLfloat rotation_y) {
  glPushMatrix();
  {
    glTranslatef(x, y, z);
    glRotatef(rotation_y, 0, 1, 0);
    glScalef(0.03f, 0.03f, 0.03f);
    glCallList(battle_droid_model_display_list_id);
  }
  glPopMatrix();
}

void draw_sun(GLfloat x, GLfloat y, GLfloat z, GLfloat scale,
              const GLfloat emission[]) {
  glPushMatrix();
  glPushAttrib(GL_LIGHTING_BIT);
  {
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    glCallList(sun_model_display_list_id);
  }
  glPopAttrib();
  glPopMatrix();
}

void draw_sun_glow(GLfloat x, GLfloat y, GLfloat z, GLfloat size,
                   GLfloat intensity) {
  GLfloat right_x = -camera_right_x;
  GLfloat right_y = -camera_right_y;
  GLfloat right_z = -camera_right_z;
  const GLfloat right_length =
      sqrtf(right_x * right_x + right_y * right_y + right_z * right_z);
  right_x /= right_length;
  right_y /= right_length;
  right_z /= right_length;

  GLfloat up_x = right_y * camera_direction_z - right_z * camera_direction_y;
  GLfloat up_y = right_z * camera_direction_x - right_x * camera_direction_z;
  GLfloat up_z = right_x * camera_direction_y - right_y * camera_direction_x;
  const GLfloat up_length = sqrtf(up_x * up_x + up_y * up_y + up_z * up_z);
  up_x /= up_length;
  up_y /= up_length;
  up_z /= up_length;

  const GLfloat half_size = size * 0.5f;
  right_x *= half_size;
  right_y *= half_size;
  right_z *= half_size;
  up_x *= half_size;
  up_y *= half_size;
  up_z *= half_size;

  glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT |
               GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT);
  {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, sun_glow_texture_id);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glColor4f(1.0f, 1.0f, 1.0f, intensity);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(x - right_x - up_x, y - right_y - up_y, z - right_z - up_z);
    glTexCoord2f(1, 0);
    glVertex3f(x + right_x - up_x, y + right_y - up_y, z + right_z - up_z);
    glTexCoord2f(1, 1);
    glVertex3f(x + right_x + up_x, y + right_y + up_y, z + right_z + up_z);
    glTexCoord2f(0, 1);
    glVertex3f(x - right_x + up_x, y - right_y + up_y, z - right_z + up_z);
    glEnd();
  }
  glPopAttrib();
}

void draw_jedi(GLfloat x, GLfloat y, GLfloat z, GLfloat rotation_y) {
  glPushMatrix();
  {
    glTranslatef(x, y, z);
    glRotatef(rotation_y, 0, 1, 0);
    glScalef(0.8, 0.8, 0.8);
    glCallList(jedi_model_display_list_id);
  }
  glPopMatrix();
}

void draw_anakin(GLfloat x, GLfloat y, GLfloat z, GLfloat rotation_y,
                 GLfloat scale) {
  glPushMatrix();
  {
    glTranslatef(x, y, z);
    glRotatef(rotation_y, 0, 1, 0);
    glScalef(scale, scale, scale);
    glCallList(anakin_model_display_list_id);
  }
  glPopMatrix();
}

void draw_canyon(GLfloat x, GLfloat y, GLfloat z, GLfloat rotation_y,
                 GLfloat scale) {
  glPushMatrix();
  {
    glTranslatef(x, y, z);
    glRotatef(rotation_y, 0, 1, 0);
    glScalef(scale, scale, scale);
    glCallList(canyon_model_display_list_id);
  }
  glPopMatrix();
}

void draw_r2d2(GLfloat x, GLfloat y, GLfloat z, GLfloat rotation_y) {
  glPushMatrix();
  {
    glTranslatef(x, y, z);
    glRotatef(rotation_y, 0, 1, 0);
    glScalef(1, 1, 1);
    glCallList(r2d2_model_display_list_id);
  }
  glPopMatrix();
}

void draw_skybox() {
  const GLint kSkyboxSlices = 64;
  const GLint kSkyboxStacks = 32;
  const GLfloat kSkyboxRadius = 100;
  const GLfloat kPi = 3.14159265f;

  glPushMatrix();
  glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT |
               GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT);
  {
    glTranslatef(camera_eye_x, camera_eye_y, camera_eye_z);
    glRotatef(-103.8f, 0, 1, 0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE);
    glColor3ub(255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, skybox_texture_id);

    for (GLint stack = 0; stack < kSkyboxStacks; stack++) {
      const GLfloat latitude_1 = (-kPi / 2) + (kPi * stack / kSkyboxStacks);
      const GLfloat latitude_2 =
          (-kPi / 2) + (kPi * (stack + 1) / kSkyboxStacks);

      const GLfloat y_1 = sinf(latitude_1);
      const GLfloat y_2 = sinf(latitude_2);
      const GLfloat radius_1 = cosf(latitude_1);
      const GLfloat radius_2 = cosf(latitude_2);

      const GLfloat texture_v_1 = 0.82f - latitude_1 * (0.82f / (kPi / 2));
      const GLfloat texture_v_2 = 0.82f - latitude_2 * (0.82f / (kPi / 2));

      glBegin(GL_QUAD_STRIP);
      for (GLint slice = 0; slice <= kSkyboxSlices; slice++) {
        const GLfloat longitude = 2 * kPi * slice / kSkyboxSlices;
        const GLfloat texture_u = 0.5f + (GLfloat)slice / kSkyboxSlices;
        const GLfloat direction_x = sinf(longitude);
        const GLfloat direction_z = -cosf(longitude);

        glTexCoord2f(texture_u, texture_v_1);
        glVertex3f(kSkyboxRadius * radius_1 * direction_x, kSkyboxRadius * y_1,
                   kSkyboxRadius * radius_1 * direction_z);

        glTexCoord2f(texture_u, texture_v_2);
        glVertex3f(kSkyboxRadius * radius_2 * direction_x, kSkyboxRadius * y_2,
                   kSkyboxRadius * radius_2 * direction_z);
      }
      glEnd();
    }
  }
  glPopAttrib();
  glPopMatrix();
}

void draw(GLFWwindow *window) {
  int window_width, window_height;
  glfwGetFramebufferSize(window, &window_width, &window_height);
  GLdouble aspect_ratio = (GLdouble)window_width / window_height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  GLdouble field_of_view = kPerspectiveFieldOfViewAngle;
  GLdouble near = kPerspectiveNearZ;
  GLdouble far = kPerspectiveFarZ;
  gluPerspective(field_of_view, aspect_ratio, near, far);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLfloat camera_center_x = (camera_eye_x + camera_direction_x);
  GLfloat camera_center_y = (camera_eye_y + camera_direction_y);
  GLfloat camera_center_z = (camera_eye_z + camera_direction_z);
  gluLookAt(camera_eye_x, camera_eye_y, camera_eye_z,          // eye
            camera_center_x, camera_center_y, camera_center_z, // direction
            kCameraUpX, kCameraUpY, kCameraUpZ                 // up
  );

  glLightfv(GL_LIGHT0, GL_POSITION, primary_sun_position);
  glLightfv(GL_LIGHT1, GL_POSITION, secondary_sun_position);

  draw_skybox();

  draw_canyon(-2.84f, -0.61f, -31.9f, -103.8f, 3.0f);

  // AT-ST (X, Y, Z, rotacao Y e escala).
  draw_at_st(-2.0f, -0.4f, -20.0f, 45.0f, 2600);
  draw_at_st(-7.0f, 1.0f, -31.0f, 45.0f, 3000);

  // Droides B1 (X, Y, Z e rotacao Y).
  draw_battle_droid(-2.0f, -0.6f, -10.0f, -90.9f);
  draw_battle_droid(0.0f, -0.6f, -10.0f, -90.9f);
  draw_battle_droid(2.0f, -0.6f, -10.0f, -90.9f);

  draw_battle_droid(-2.0f, -0.6f, -12.0f, -90.9f);
  draw_battle_droid(0.0f, -0.6f, -12.0f, -90.9f);
  draw_battle_droid(2.0f, -0.6f, -12.0f, -90.9f);

  draw_battle_droid(-2.0f, -0.6f, -14.0f, -90.9f);
  draw_battle_droid(0.0f, -0.6f, -14.0f, -90.9f);
  draw_battle_droid(2.0f, -0.6f, -14.0f, -90.9f);

  draw_battle_droid(-2.0f, -0.6f, -16.0f, -90.9f);
  draw_battle_droid(0.0f, -0.6f, -16.0f, -90.9f);
  draw_battle_droid(2.0f, -0.6f, -16.0f, -90.9f);

  // Jedi, Anakin e R2-D2.
  draw_jedi(0.3f, 0.3f, -3.2f, 180.0f);
  draw_anakin(-1.0f, -0.41f, -3.2f, 180.0f, 0.15f);
  draw_r2d2(1.0f, -0.5f, -1.8f, 180.0f);

  // Estrela da Morte.
  glPushMatrix();
  {
    glTranslatef(45.0f, 45.0f, -75.0f);
    glRotatef(-20.0f, 0, 1, 0);
    glRotatef(30.0f, 1, 0, 0);
    glScalef(0.1, 0.1, 0.1);
    glCallList(death_star_model_display_list_id);
  }
  glPopMatrix();

  // Sois de Tatooine.
  draw_sun(primary_sun_position[0], primary_sun_position[1],
           primary_sun_position[2], 4.0f, kPrimarySunEmission);
  draw_sun(secondary_sun_position[0], secondary_sun_position[1],
           secondary_sun_position[2], 2.0f, kSecondarySunEmission);
  draw_sun_glow(primary_sun_position[0], primary_sun_position[1],
                primary_sun_position[2], 20.0f, 0.80f);
  draw_sun_glow(secondary_sun_position[0], secondary_sun_position[1],
                secondary_sun_position[2], 8.0f, 0.70f);
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Falha ao inicializar GLFW" << std::endl;
    return EXIT_FAILURE;
  }

  GLFWwindow *window = glfwCreateWindow(800, 800, "", NULL, NULL);
  if (!window) {
    std::cerr << "Falha ao criar a janela GLFW" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwSetWindowPos(window, 0, 0);
  glfwSetWindowTitle(window, "Lesson 12 - Cena 3D Iluminada");
  glfwMakeContextCurrent(window);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_read);

  if (glewInit() != GLEW_OK) {
    std::cerr << "Falha ao inicializar GLEW" << std::endl;
    return EXIT_FAILURE;
  }

  load_texture(skybox_texture_id, "skybox/skb_desert_landscape.png");
  glBindTexture(GL_TEXTURE_2D, skybox_texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  create_sun_glow_texture();
  load_model(death_star_model, "models/death_star.glb");
  load_model(at_st_model, "models/at-st_star_wars.glb");
  load_model(battle_droid_model, "models/battle_droid.glb");
  load_model(sun_model, "models/the_sun.glb");
  load_model(jedi_model, "models/jedi_knight.glb");
  load_model(anakin_model, "models/anakin_skywalker.glb");
  load_model(r2d2_model, "models/star_wars_r2-d2.glb");
  load_model(canyon_model, "models/red_sand_desert_canyon.glb");

  create_model_display_list(death_star_model_display_list_id, death_star_model);
  create_model_display_list(at_st_model_display_list_id, at_st_model);
  create_model_display_list(battle_droid_model_display_list_id,
                            battle_droid_model);
  create_model_display_list(sun_model_display_list_id, sun_model);
  create_model_display_list(jedi_model_display_list_id, jedi_model);
  create_model_display_list(anakin_model_display_list_id, anakin_model);
  create_model_display_list(r2d2_model_display_list_id, r2d2_model);
  create_model_display_list(canyon_model_display_list_id, canyon_model);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, kPrimarySunAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, kPrimarySunDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, kPrimarySunSpecular);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, kSunConstantAttenuation);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, kSunLinearAttenuation);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, kSunQuadraticAttenuation);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, kSecondarySunAmbient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, kSecondarySunDiffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, kSecondarySunSpecular);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, kSunConstantAttenuation);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, kSunLinearAttenuation);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, kSunQuadraticAttenuation);

  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kSceneSpecular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, kSceneShininess);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(kSkyColorDay[0], kSkyColorDay[1], kSkyColorDay[2], 1);
    // glClearColor(kSkyColorNight[0], kSkyColorNight[1], kSkyColorNight[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    keyboard_read(window);
    resize_window(window);
    update_orbiting_sun_position();
    draw(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteLists(death_star_model_display_list_id, 1);
  glDeleteLists(at_st_model_display_list_id, 1);
  glDeleteLists(battle_droid_model_display_list_id, 1);
  glDeleteLists(sun_model_display_list_id, 1);
  glDeleteLists(jedi_model_display_list_id, 1);
  glDeleteLists(anakin_model_display_list_id, 1);
  glDeleteLists(r2d2_model_display_list_id, 1);
  glDeleteLists(canyon_model_display_list_id, 1);
  destroy_model(death_star_model);
  destroy_model(at_st_model);
  destroy_model(battle_droid_model);
  destroy_model(sun_model);
  destroy_model(jedi_model);
  destroy_model(anakin_model);
  destroy_model(r2d2_model);
  destroy_model(canyon_model);
  glDeleteTextures(1, &skybox_texture_id);
  glDeleteTextures(1, &sun_glow_texture_id);
  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}
