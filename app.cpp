//
// Created by Garrett on 12/24/2021.
//

#include "app.hpp"
#include "render_system.hpp"
#include "keyboard_movement_controller.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>

app::app()
{
    load_game_objects();
}

app::~app()
{
}

void app::run()
{
    render_system system{this->device_, this->renderer_.get_swap_chain_render_pass()};
    camera c{};
//    c.set_view_direction(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 10.f));

    auto viewer_object = game_object::create_game_object();
    keyboard_movement_controller camera_controller{};
//    c.set_view_target(glm::vec3(-1.f, -2.f, 20.f), glm::vec3(0.f, 0.f, 5.f));

    auto current_time = chrono::high_resolution_clock::now();

    while (!this->window_.close()) {
        glfwPollEvents();

        auto new_time = chrono::high_resolution_clock::now();
        auto frame_time = chrono::duration<float, chrono::seconds::period>(new_time - current_time).count();
        current_time = new_time;

        camera_controller.move_in_plane_xz(this->window_.get_GLFWWindow(), frame_time, viewer_object);
        c.set_view_yxz(viewer_object.transform_.translation, viewer_object.transform_.rotation);

        float aspect = renderer_.get_aspect_ratio();
//            c.set_orthographic_projection(-aspect, aspect, -1, 1, -1, 1);
        c.set_perspective_projection(glm::radians(50.f), aspect, 0.1f, 10.f);

        if (auto command_buffer = renderer_.begin_frame()) {
            renderer_.begin_swap_chain_render_pass(command_buffer);
            system.render_game_objects(command_buffer, this->game_objects, c);
            renderer_.end_swap_chain_render_pass(command_buffer);
            renderer_.end_frame();
        }
    }

    vkDeviceWaitIdle(this->device_.dev());
}

void app::load_game_objects()
{
    shared_ptr<model> cube_model = model::create_model_from_file(device_, "models/smooth_vase.obj");

    auto cube = game_object::create_game_object();
    cube.model_ = cube_model;
    cube.transform_.translation = {0.f, 0.f, 2.5f};
    cube.transform_.scale = {2.5f, 2.5f, 2.5f};

    this->game_objects.push_back(move(cube));
}