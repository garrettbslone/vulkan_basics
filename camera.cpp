//
// Created by Garrett on 12/27/2021.
//

#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <limits>

void camera::set_orthographic_projection(float left, float right, float top, float bottom, float near, float far)
{
    this->projection_matrix = glm::ortho(left, right, bottom, top, near, far);
}

void camera::set_perspective_projection(float fovy, float aspect, float near, float far)
{
    this->projection_matrix = glm::perspective(fovy, aspect, near, far);
}
