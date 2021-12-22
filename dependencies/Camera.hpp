#include "glad/glad.h"
#include <glm/glm.hpp>
#include <iostream>

// default configuration
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 1.5f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;

glm::vec3 D_POSITION = glm::vec3(0.0f, 0.0f, 15.0f);
const glm::vec3 D_UP = glm::vec3(0.0f, 1.0f, 0.0f);
// direction enum
enum Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// camera class

class Camera {
public:
    glm::vec3 Position, Front, Up, Right, WorldUp;
    
    float Yaw, Pitch;
    float MovementSpeed, MouseSensitivity;
    float Zoom;

    // constructor for vector
    // ----------------------

    Camera(
        glm::vec3 position = D_POSITION,
        glm::vec3 up = D_UP,
        float yaw = YAW, float pitch = PITCH,
        float speed = SPEED, float sensitivity = SENSITIVITY,
        float zoom = ZOOM
    )
        :Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(speed),
        MouseSensitivity(sensitivity),
        Zoom(zoom),
        Position(position),
        WorldUp(up),
        Yaw(yaw),
        Pitch(pitch)
    {
        updateCameraVectors();
    }

    // destructor 
    // ----------

    ~Camera () {
    }

    // get the view matrix 
    // -------------------

    glm::mat4 GetViewMatrix () {
        return glm::lookAt(Position,Front + Position, Up);
    }

    // handle inputs
    // -------------

    void ProcessMouseMovement (float x_offset, float y_offset) {
        Yaw += MouseSensitivity * x_offset;
        Pitch += MouseSensitivity * y_offset;

        Pitch = Pitch > 80.0f ? 80.0f : Pitch < -80.0f ? -80.0f : Pitch;

        updateCameraVectors();
    }

    void ProcessKeyboard (Direction direction, float deltaTime) {
        float distance = MovementSpeed * deltaTime;

        glm::vec3 groundFront = glm::normalize(glm::vec3(Front.x, 0.0f, Front.z));
        glm::vec3 groundRight = glm::normalize(glm::vec3(Right.x, 0.0f, Right.z));
        glm::vec3 groundUp = glm::normalize(WorldUp);

        switch (direction)
        {
        case FORWARD:
            Position += distance * groundFront;
            break;

        case BACKWARD:
            Position -= distance * groundFront;
            break;

        case LEFT:
            Position -= distance * groundRight;
            break;

        case RIGHT:
            Position += distance * groundRight;
            break;

        case UP:
            Position += distance * groundUp;
            break;

        case DOWN:
            Position -= distance * groundUp;
            break;

        default:
            break;
        }
        updateCameraVectors();
    }


private:
    // update all vectors based on parameters
    // --------------------------------------

    void updateCameraVectors () {
        glm::vec3 front = glm::vec3 (
            cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)),
            sin(glm::radians(Pitch)),
            sin(glm::radians(Yaw)) * cos(glm::radians(Pitch))
        );

        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

