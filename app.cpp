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

struct global_ubo {
    glm::mat4 projection_view{1.f};
    glm::vec4 ambient_light_color{1.f, 1.f, 1.f, 0.02f}; // w is intensity
    glm::vec4 light_position{-1.f}; // ignore w -- for alignment purposes
    glm::vec4 light_color{1.f}; // w is intensity
};

app::app()
{
    global_pool = descriptor_pool::Builder(device_)
        .setMaxSets(swap_chain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, swap_chain::MAX_FRAMES_IN_FLIGHT)
        .build();

    load_game_objects();
}

app::~app()
{
}

void app::run()
{
    vector<unique_ptr<buffer>> ubo_buffers{swap_chain::MAX_FRAMES_IN_FLIGHT};
    for (auto i = 0; i < ubo_buffers.size(); i++) {
        ubo_buffers[i] = make_unique<buffer>(
            device_,
            sizeof(global_ubo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        ubo_buffers[i]->map();
    }

    auto global_set_layout = descriptor_set_layout::Builder(device_)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
        .build();

    vector<VkDescriptorSet> global_descriptor_sets(swap_chain::MAX_FRAMES_IN_FLIGHT);
    for (auto i = 0; i < global_descriptor_sets.size(); i++) {
        auto buffer_info = ubo_buffers[i]->descriptorInfo();
        descriptor_writer(*global_set_layout, *global_pool)
        .writeBuffer(0, &buffer_info)
        .build(global_descriptor_sets[i]);
    }

    render_system system{this->device_, this->renderer_.get_swap_chain_render_pass(), global_set_layout->getDescriptorSetLayout()};
    camera c{};
//    c.set_view_direction(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 10.f));

    auto viewer_object = game_object::create_game_object();
    viewer_object.transform_.translation.z = -2.5f;
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
        c.set_perspective_projection(glm::radians(50.f), aspect, 0.1f, 100.f);

        if (auto command_buffer = renderer_.begin_frame()) {
            int frame_index = renderer_.get_frame_index();
            frame_info info = {
                    frame_index,
                    frame_time,
                    command_buffer,
                    c,
                    global_descriptor_sets[frame_index],
                    this->game_objects
            };

            // update
            global_ubo ubo{};
            ubo.projection_view = c.get_projection() * c.get_view();
            ubo_buffers[frame_index]->writeToBuffer(&ubo);
            ubo_buffers[frame_index]->flush();

            // render
            renderer_.begin_swap_chain_render_pass(command_buffer);
            system.render_game_objects(info);
            renderer_.end_swap_chain_render_pass(command_buffer);
            renderer_.end_frame();
        }
    }

    vkDeviceWaitIdle(this->device_.dev());
}

void app::load_game_objects()
{
    shared_ptr<model> obj_model = model::create_model_from_file(device_, "models/smooth_vase.obj");

    auto smooth_vase = game_object::create_game_object();
    smooth_vase.model_ = obj_model;
    smooth_vase.transform_.translation = {-0.5f, 0.5f, 0.f};
    smooth_vase.transform_.scale = {2.5f, 2.5f, 2.5f};
    this->game_objects.emplace(smooth_vase.get_id(), move(smooth_vase));


    obj_model = model::create_model_from_file(device_, "models/flat_vase.obj");

    auto flat_vase = game_object::create_game_object();
    flat_vase.model_ = obj_model;
    flat_vase.transform_.translation = {0.5f, 0.5f, 0.f};
    flat_vase.transform_.scale = {2.5f, 2.5f, 2.5f};

    this->game_objects.emplace(flat_vase.get_id(), move(flat_vase));

    obj_model = model::create_model_from_file(device_, "models/quad.obj");

    auto floor = game_object::create_game_object();
    floor.model_ = obj_model;
    floor.transform_.translation = {0.f, 0.5f, 0.f};
    floor.transform_.scale = {4.f, 2.5f, 4.f};

    this->game_objects.emplace(floor.get_id(), move(floor));
}