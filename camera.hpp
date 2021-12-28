//
// Created by Garrett on 12/27/2021.
//

#ifndef VK_BASICS_CAMERA_HPP
#define VK_BASICS_CAMERA_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class camera {
public:
    void set_orthographic_projection(float left, float right, float top, float bottom, float near, float far);
    void set_perspective_projection(float fovy, float aspect, float near, float far);

    void set_view_direction(glm::vec3 position, glm::vec3 direction, glm::vec3 up = {0.f, -1.f, 0.f});
    void set_view_target(glm::vec3 position, glm::vec3 target, glm::vec3 up = {0.f, -1.f, 0.f});
    void set_view_yxz(glm::vec3 position, glm::vec3 rotation);

    inline const glm::mat4 &get_projection() const { return this->projection_matrix; }
    inline const glm::mat4 &get_view() const { return this->view_matrix; }

private:
    glm::mat4 projection_matrix{1.f};
    glm::mat4 view_matrix{1.f};
};

#endif //VK_BASICS_CAMERA_HPP
