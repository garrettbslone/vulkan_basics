//
// Created by Garrett on 12/26/2021.
//

#include "renderer.hpp"

#include <array>
#include <stdexcept>

renderer::renderer(window &w, device &d)
    : window_{w}, device_{d}
{
    recreate_swap_chain();
    create_command_buffers();
}

renderer::~renderer()
{
    free_command_buffers();
}

void renderer::create_command_buffers()
{
    this->command_buffers.resize(swap_chain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo alloc_info{};

    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = this->device_.getCommandPool();
    alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());

    if (vkAllocateCommandBuffers(this->device_.dev(), &alloc_info, command_buffers.data()) != VK_SUCCESS) {
        throw runtime_error("failed to allocate command buffers");
    }
}

void renderer::free_command_buffers()
{
    vkFreeCommandBuffers(
        this->device_.dev(),
        this->device_.getCommandPool(),
        static_cast<uint32_t>(this->command_buffers.size()),
        this->command_buffers.data());

    command_buffers.clear();
}

void renderer::recreate_swap_chain()
{
    auto extent = this->window_.get_extent();

    while (extent.width == 0 || extent.height == 0) {
        extent = this->window_.get_extent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(this->device_.dev());

    if (!this->swap_chain_)
        this->swap_chain_ = std::make_unique<swap_chain>(this->device_, extent);
    else {
        shared_ptr<swap_chain> old_swap_chain = move(this->swap_chain_);
        this->swap_chain_ = std::make_unique<swap_chain>(this->device_, extent, old_swap_chain);

        if (!old_swap_chain->compare_swap_formats(*this->swap_chain_.get())) {
            throw runtime_error("Swap chain image(or depth) format has changed!");
        }
    }
}

VkCommandBuffer renderer::begin_frame()
{
    assert(!is_frame_started && "Can't call begin_frame while already in progress");

    auto res = this->swap_chain_->acquireNextImage(&current_image_index);

    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swap_chain();
        return nullptr;
    } else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
        throw runtime_error("failed to acquire swap chain image!");
    }

    is_frame_started = true;

    auto command_buffer = get_current_command_buffer();
    VkCommandBufferBeginInfo begin_info{};

    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        throw runtime_error("failed to begin recording command buffer!");
    }

    return command_buffer;
}

void renderer::end_frame()
{
    assert(is_frame_started && "Can't call end_frame while frame is not in progress");

    auto command_buffer = get_current_command_buffer();
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw runtime_error("failed to record command buffer!");
    }

    auto res = this->swap_chain_->submitCommandBuffers(&command_buffer, &current_image_index);

    if (res == VK_ERROR_OUT_OF_DATE_KHR || this->window_.is_framebuffer_resized()) {
        this->window_.set_framebuffer_resized(false);
        recreate_swap_chain();
    } else if (res != VK_SUCCESS) {
        throw runtime_error("failed to acquire swap chain image!");
    }

    is_frame_started = false;
    current_frame_index = (current_frame_index + 1) % swap_chain::MAX_FRAMES_IN_FLIGHT;
}

void renderer::begin_swap_chain_render_pass(VkCommandBuffer command_buffer)
{
    assert(is_frame_started && "Can't call begin_swap_chain_render_pass while already in progress");
    assert(command_buffer == get_current_command_buffer() && "Can't begin render pass on command buffer from a different frame!");

    VkRenderPassBeginInfo render_pass_info{};

    VkRect2D scissor{{0, 0}, this->swap_chain_->getSwapChainExtent()};
    VkViewport viewport{};

    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = static_cast<float>(this->swap_chain_->getSwapChainExtent().width);
    viewport.height = static_cast<float>(this->swap_chain_->getSwapChainExtent().height);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor),

    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = this->swap_chain_->getRenderPass();
    render_pass_info.framebuffer = this->swap_chain_->getFrameBuffer(current_image_index);
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = this->swap_chain_->getSwapChainExtent();

    std::array<VkClearValue, 2> clear_vals{};

    clear_vals[0].color = {0.01f, 0.01f, 0.01f, 1.f};
    clear_vals[1].depthStencil = {1.f, 0};

    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_vals.size());
    render_pass_info.pClearValues = clear_vals.data();

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void renderer::end_swap_chain_render_pass(VkCommandBuffer command_buffer)
{
    assert(is_frame_started && "Can't call end_swap_chain_render_pass while already in progress");
    assert(command_buffer == get_current_command_buffer() && "Can't end_swap_chain_render_pass on command buffer from a different frame!");
    vkCmdEndRenderPass(command_buffer);
}
