//
// Created by Garrett on 12/27/2021.
//

#include "keyboard_movement_controller.hpp"

void keyboard_movement_controller::move_in_plane_xz(GLFWwindow *w, float dt, game_object &game_object_)
{
    glm::vec3 rotate{0};

    if (glfwGetKey(w, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
    if (glfwGetKey(w, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
    if (glfwGetKey(w, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
    if (glfwGetKey(w, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        game_object_.transform_.rotation += lookspeed * dt * glm::normalize(rotate);
    }

    game_object_.transform_.rotation.x = glm::clamp(game_object_.transform_.rotation.x, -1.5f, 1.5f);
    game_object_.transform_.rotation.y = glm::mod(game_object_.transform_.rotation.y, glm::two_pi<float>());

    float yaw = game_object_.transform_.rotation.y;
    const glm::vec3 forward_dir{glm::sin(yaw), 0.f, glm::cos(yaw)};
    const glm::vec3 right_dir{forward_dir.z, 0.f, -forward_dir.x};
    const glm::vec3 up_dir{0.f, -1.f, 0.f};
    glm::vec3 move_dir{0.f};

    if (glfwGetKey(w, keys.moveForward) == GLFW_PRESS) move_dir += forward_dir;
    if (glfwGetKey(w, keys.moveBackward) == GLFW_PRESS) move_dir -= forward_dir;
    if (glfwGetKey(w, keys.moveRight) == GLFW_PRESS) move_dir += right_dir;
    if (glfwGetKey(w, keys.moveLeft) == GLFW_PRESS) move_dir -= right_dir;
    if (glfwGetKey(w, keys.moveUp) == GLFW_PRESS) move_dir += up_dir;
    if (glfwGetKey(w, keys.moveDown) == GLFW_PRESS) move_dir -= up_dir;

    if (glm::dot(move_dir, move_dir) > std::numeric_limits<float>::epsilon()) {
        game_object_.transform_.translation += move_speed * dt * glm::normalize(move_dir);
    }
}
