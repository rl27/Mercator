#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float DEFAULT_YAW = -90.0f;
const float DEFAULT_PITCH = 0.0f;
const float DEFAULT_SPEED = 0.8f;
const float DEFAULT_SENSITIVITY = 0.1f;
const float DEFAULT_FOV = 60.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float FOV;
    bool sprint;

    // Constructor that takes vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH);
    // Constructor that takes scalars
    Camera(float posX = 0.0f, float posY = 0.0f, float posZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f, float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH);
    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();
    // Updates camera position on input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime, bool FPS = false);
    // Updates Euler Angles (Yaw/Pitch) on input received from mouse movement, then updates camera.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    // Updates FOV on input received from a mouse scrolling. Only requires input on the vertical wheel-axis.
    void ProcessMouseScroll(float yoffset);
    // Move faster when shift key held down
    void StartSprint();
    // Move normally when shift key not held down
    void EndSprint();
private:
    // Updates Front/Right/Up camera vectors from the Euler Angles (Yaw/Pitch/Roll)
    void updateCameraVectors();
};

#endif