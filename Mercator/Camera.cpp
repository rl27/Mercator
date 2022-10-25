#include "Camera.h"

Camera::Camera(glm::dvec3 position, glm::dvec3 up, double yaw, double pitch) : Front(glm::dvec3(1.0, 0.0, 0.0)), MovementSpeed(DEFAULT_SPEED), MouseSensitivity(DEFAULT_SENSITIVITY), FOV(DEFAULT_FOV)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    sprint = false;
    updateCameraVectors();
}

Camera::Camera(double posX, double posY, double posZ, double upX, double upY, double upZ, double yaw, double pitch) : Front(glm::dvec3(0.0, 0.0, -1.0)), MovementSpeed(DEFAULT_SPEED), MouseSensitivity(DEFAULT_SENSITIVITY), FOV(DEFAULT_FOV)
{
    Position = glm::dvec3(posX, posY, posZ);
    WorldUp = glm::dvec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    sprint = false;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    //return glm::lookAt(Position, Position + Front, Up);
    return glm::lookAt(glm::dvec3(0.0, 0.4, 0.0), glm::dvec3(0.0, 0.4, 0.0) + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, double deltaTime, bool FPS)
{
    double velocity = MovementSpeed * deltaTime;

    glm::dvec3 myFront = Front;

    double prevY = Position.y;

    // Full speed front movement, regardless of how high you look
    if (FPS)
    {
        myFront.y = 0;
        myFront = glm::normalize(myFront);
    }

    if (direction == FORWARD)
        Position += myFront * velocity;
        //Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= myFront * velocity;
        //Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;

    if (FPS)
        Position.y = prevY;
}

void Camera::ProcessMouseMovement(double xoffset, double yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(double yoffset)
{
    FOV -= (double)yoffset;
    if (FOV < 1.0f)
        FOV = 1.0f;
    if (FOV > 60.0f)
        FOV = 60.0f;
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::dvec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::StartSprint()
{
    if (!sprint)
    {
        sprint = true;
        MovementSpeed *= 1.6f;
    }
}

void Camera::EndSprint()
{
    if (sprint)
    {
        sprint = false;
        MovementSpeed *= 0.625f;
    }
}