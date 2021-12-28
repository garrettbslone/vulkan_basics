//
// Created by Garrett on 12/26/2021.
//

#ifndef VK_BASICS_RENDER_SYSTEM_HPP
#define VK_BASICS_RENDER_SYSTEM_HPP

#include "window.hpp"
#include "pipeline.hpp"
#include "model.hpp"
#include "game_object.hpp"
#include "renderer.hpp"

#include <memory>

class render_system {
public:
    render_system(device &d, VkRenderPass render_pass);
    ~render_system();

    render_system(const render_system &) = delete;
    render_system &operator=(const render_system &) = delete;

    void run();

    void render_game_objects(VkCommandBuffer command_buffer, vector<game_object> &objects);

private:
    void create_pipeline_layout();
    void create_pipeline(VkRenderPass render_pass);

    device &device_;
    std::unique_ptr<pipeline> pipeline_;
    VkPipelineLayout pipeline_layout;
};

#endif //VK_BASICS_RENDER_SYSTEM_HPP
