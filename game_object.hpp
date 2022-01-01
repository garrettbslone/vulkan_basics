//
// Created by Garrett on 12/26/2021.
//

#ifndef VK_BASICS_GAME_OBJECT_HPP
#define VK_BASICS_GAME_OBJECT_HPP

#include "model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <unordered_map>
#include <memory>

struct transform_component {
    glm::vec3 translation{};
    glm::vec3 scale{1.f, 1.f, 1.f};
    glm::vec3 rotation{};

    glm::mat4 mat4();
    glm::mat3 normal_matrix();
};

class game_object {
public:
    using id_t = unsigned int;
    using Map = std::unordered_map<id_t, game_object>;

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
    transform_component transform_{};

private:
    game_object(id_t obj_id) : id{obj_id} {}

    id_t id;
};

#endif //VK_BASICS_GAME_OBJECT_HPP
