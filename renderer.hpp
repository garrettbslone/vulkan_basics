//
// Created by Garrett on 12/26/2021.
//

#ifndef VK_BASICS_RENDERER_HPP
#define VK_BASICS_RENDERER_HPP

#include "window.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

#include <cassert>
#include <memory>

using namespace std;

class renderer {
public:
    renderer(window &w, device &d);
    ~renderer();

    renderer(const renderer &) = delete;
    renderer &operator=(const renderer &) = delete;

    float get_aspect_ratio() const { return swap_chain_->extentAspectRatio(); }
    bool is_frame_in_progress() const { return this->is_frame_started; }
    VkCommandBuffer get_current_command_buffer() const
    {
        assert(this->is_frame_started && "Cannot get command buffer when frame not in progress!");
        return command_buffers[current_frame_index];
    }
    VkRenderPass get_swap_chain_render_pass() const { return this->swap_chain_->getRenderPass(); }
    int get_frame_index() const
    {
        assert(is_frame_started && "Cannot get command buffer when frame not is progress!");
        return current_frame_index;
    }

    VkCommandBuffer begin_frame();
    void end_frame();

    void begin_swap_chain_render_pass(VkCommandBuffer command_buffer);
    void end_swap_chain_render_pass(VkCommandBuffer command_buffer);

private:
    void create_command_buffers();
    void free_command_buffers();
    void recreate_swap_chain();

    window &window_;
    device &device_;
    unique_ptr<swap_chain> swap_chain_;
    vector<VkCommandBuffer> command_buffers;

    uint32_t current_image_index{0};
    int current_frame_index{0};
    bool is_frame_started{false};
};

#endif //VK_BASICS_RENDERER_HPP
