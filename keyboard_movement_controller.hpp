//
// Created by Garrett on 12/27/2021.
//

#ifndef VK_BASICS_KEYBOARD_MOVEMENT_CONTROLLER_HPP
#define VK_BASICS_KEYBOARD_MOVEMENT_CONTROLLER_HPP

#include "game_object.hpp"
#include "window.hpp"

class keyboard_movement_controller {
public:
    struct key_mappings {
        int moveLeft = GLFW_KEY_A;
        int moveRight = GLFW_KEY_D;
        int moveForward = GLFW_KEY_W;
        int moveBackward = GLFW_KEY_S;
        int moveUp = GLFW_KEY_E;
        int moveDown = GLFW_KEY_Q;
        int lookLeft = GLFW_KEY_LEFT;
        int lookRight = GLFW_KEY_RIGHT;
        int lookUp = GLFW_KEY_UP;
        int lookDown = GLFW_KEY_DOWN;
    };

    void move_in_plane_xz(GLFWwindow *w, float dt, game_object &game_object_);

    key_mappings keys{};
    float move_speed{3.f};
    float lookspeed{1.5f};
};

#endif //VK_BASICS_KEYBOARD_MOVEMENT_CONTROLLER_HPP
