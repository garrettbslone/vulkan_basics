//
// Created by Garrett on 12/26/2021.
//

#ifndef VK_BASICS_GAME_OBJECT_HPP
#define VK_BASICS_GAME_OBJECT_HPP

#include "model.hpp"

#include <memory>

struct transform_2d {
    glm::vec2 translation{};
    glm::vec2 scale{1.f, 1.f};
    float rotation;

    glm::mat2 mat2()
    {
        const float s = glm::sin(rotation), c = glm::cos(rotation);

        glm::mat2 rot_mat{{c, s}, {-s, c}};
        glm::mat2 scale_mat{{scale.x, 0.f}, {0.f, scale.y}};
        return rot_mat * scale_mat;
    }
};

class game_object {
public:
    using id_t = unsigned int;

    static game_object create_game_object()
    {
        static id_t curr_id = 0;
        return game_object{curr_id++};
    }

    game_object(const game_object &) = delete;
    game_object &operator=(const game_object &) = delete;
    game_object(game_object &&) = default;
    game_object &operator=(game_object &&) = default;

    id_t get_id() const { return id; }

    std::shared_ptr<model> model_{};
    glm::vec3 color{};
    transform_2d transform_2d_{};

private:
    game_object(id_t obj_id) : id{obj_id} {}

    id_t id;
};

#endif //VK_BASICS_GAME_OBJECT_HPP
