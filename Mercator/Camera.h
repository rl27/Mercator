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
const double DEFAULT_YAW = -90.0;
const double DEFAULT_PITCH = 0.0;
const double DEFAULT_SPEED = 0.6;
const double DEFAULT_SENSITIVITY = 0.1;
const double DEFAULT_FOV = 60.0;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::dvec3 Position;
    glm::dvec3 Front;
    glm::dvec3 Up;
    glm::dvec3 Right;
    glm::dvec3 WorldUp;
    // Euler Angles
    double Yaw;
    double Pitch;
    // Camera options
    double MovementSpeed;
    double MouseSensitivity;
    double FOV;
    bool sprint;

    // Constructor that takes vectors
    Camera(glm::dvec3 position = glm::dvec3(0.0, 0.0, 0.0), glm::dvec3 up = glm::dvec3(0.0, 1.0, 0.0), double yaw = DEFAULT_YAW, double pitch = DEFAULT_PITCH);
    // Constructor that takes scalars
    Camera(double posX = 0.0, double posY = 0.0, double posZ = 0.0, double upX = 0.0, double upY = 1.0, double upZ = 0.0, double yaw = DEFAULT_YAW, double pitch = DEFAULT_PITCH);
    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();
    // Updates camera position on input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, double deltaTime, bool FPS = false);
    // Updates Euler Angles (Yaw/Pitch) on input received from mouse movement, then updates camera.
    void ProcessMouseMovement(double xoffset, double yoffset, GLboolean constrainPitch = true);
    // Updates FOV on input received from a mouse scrolling. Only requires input on the vertical wheel-axis.
    void ProcessMouseScroll(double yoffset);
    // Move faster when shift key held down
    void StartSprint();
    // Move normally when shift key not held down
    void EndSprint();
private:
    // Updates Front/Right/Up camera vectors from the Euler Angles (Yaw/Pitch/Roll)
    void updateCameraVectors();
};

#endif