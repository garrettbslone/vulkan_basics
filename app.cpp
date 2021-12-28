//
// Created by Garrett on 12/24/2021.
//

#include "app.hpp"
#include "render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

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
    while (!this->window_.close()) {
        glfwPollEvents();

        if (auto command_buffer = renderer_.begin_frame()) {
            renderer_.begin_swap_chain_render_pass(command_buffer);
            system.render_game_objects(command_buffer, this->game_objects);
            renderer_.end_swap_chain_render_pass(command_buffer);
            renderer_.end_frame();
        }
    }

    vkDeviceWaitIdle(this->device_.dev());
}

void app::sierpinski(std::vector<model::vertex> &vertices,
                     int depth,
                     glm::vec2 left,
                     glm::vec2 right,
                     glm::vec2 top)
{
    if (depth <= 0) {
        vertices.push_back({top});
        vertices.push_back({right});
        vertices.push_back({left});
    } else {
        auto leftTop = 0.5f * (left + top);
        auto rightTop = 0.5f * (right + top);
        auto leftRight = 0.5f * (left + right);
        sierpinski(vertices, depth - 1, left, leftRight, leftTop);
        sierpinski(vertices, depth - 1, leftRight, right, rightTop);
        sierpinski(vertices, depth - 1, leftTop, rightTop, top);
    }
}

void app::load_game_objects()
{
    std::vector<model::vertex> vertices{
        {{-0.5f, 0.5f},  {1.f, 0.f, 0.f}},
        {{0.5f, 0.5f}, {0.f, 1.f, 0.f}},
        {{0.0f, -0.5f}, { 0.f, 0.f, 1.f}}
    };
//    sierpinski(vertices, 3, {-0.5f, 0.5f}, {0.5f, 0.5f}, {0.0f, -0.5f});
    auto model_ = std::make_shared<model>(&this->device_, vertices);
    auto triangle = game_object::create_game_object();

    triangle.model_ = model_;
    triangle.color = {0.1f, 0.8f, 0.1f};
    triangle.transform_2d_.translation.x = 0.2f;
    triangle.transform_2d_.scale = {2.f, 0.5f};
    triangle.transform_2d_.rotation = 0.25f * glm::two_pi<float>();

    this->game_objects.push_back(std::move(triangle));

//    std::vector<model::vertex> vertices{
//        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
//        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
//    auto model_ = std::make_shared<model>(&device_, vertices);
//
//    // https://www.color-hex.com/color-palette/5361
//    std::vector<glm::vec3> colors{
//        {1.f, .7f, .73f},
//        {1.f, .87f, .73f},
//        {1.f, 1.f, .73f},
//        {.73f, 1.f, .8f},
//        {.73, .88f, 1.f}  //
//    };
//    for (auto& color : colors) {
//        color = glm::pow(color, glm::vec3{2.2f});
//    }
//    for (int i = 0; i < 40; i++) {
//        auto triangle = game_object::create_game_object();
//        triangle.model_ = model_;
//        triangle.transform_2d_.scale = glm::vec2(.5f) + i * 0.025f;
//        triangle.transform_2d_.rotation = i * glm::pi<float>() * .025f;
//        triangle.color = colors[i % colors.size()];
//        game_objects.push_back(std::move(triangle));
//    }
}

//void app::create_pipeline_layout()
//{
//    VkPushConstantRange push_constant_range{};
//
//    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
//    push_constant_range.offset = 0;
//    push_constant_range.size = sizeof(push_constant_data);
//
//    VkPipelineLayoutCreateInfo pipeline_layout_info{};
//
//    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//    pipeline_layout_info.setLayoutCount = 0;
//    pipeline_layout_info.pSetLayouts = nullptr;
//    pipeline_layout_info.pushConstantRangeCount = 1;
//    pipeline_layout_info.pPushConstantRanges = &push_constant_range;
//
//    if (vkCreatePipelineLayout(this->device_.dev(), &pipeline_layout_info, nullptr, &this->pipeline_layout) != VK_SUCCESS) {
//        throw runtime_error("failed to create pipeline layout!");
//    }
//}
//
//void app::create_pipeline()
//{
//    pipeline_config_info pipeline_config{};
//    pipeline::default_config_info(pipeline_config);
//
//    pipeline_config.render_pass = this->renderer_.get_swap_chain_render_pass();
//    pipeline_config.pipeline_layout = this->pipeline_layout;
//
//    this->pipeline_ = std::make_unique<pipeline>(
//        &this->device_,
//        pipeline_config,
//        "shaders/shader.vert.spv",
//        "shaders/shader.frag.spv"
//        );
//}

//void app::create_command_buffers()
//{
//    this->command_buffers.resize(this->swap_chain_->imageCount());
//
//    VkCommandBufferAllocateInfo alloc_info{};
//
//    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    alloc_info.commandPool = this->device_.getCommandPool();
//    alloc_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());
//
//    if (vkAllocateCommandBuffers(this->device_.dev(), &alloc_info, command_buffers.data()) != VK_SUCCESS) {
//        throw runtime_error("failed to allocate command buffers");
//    }
//}

//void app::free_command_buffers()
//{
//    vkFreeCommandBuffers(
//        this->device_.dev(),
//        this->device_.getCommandPool(),
//        static_cast<uint32_t>(this->command_buffers.size()),
//        this->command_buffers.data());
//
//    command_buffers.clear();
//}

//void app::draw_frame(double dt)
//{
//    uint32_t image_ind;
//    auto res = this->swap_chain_->acquireNextImage(&image_ind);
//
//    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
////        this->swap_chain_->recreate();
////        create_pipeline();
////        create_command_buffers();
//        recreate_swap_chain();
//        return;
//    } else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
//        throw runtime_error("failed to acquire swap chain image!");
//    }
//
//    record_command_buffer(image_ind, dt);
//    res = this->swap_chain_->submitCommandBuffers(&command_buffers[image_ind], &image_ind);
//
//    if (res == VK_ERROR_OUT_OF_DATE_KHR || this->window_.is_framebuffer_resized()) {
////        this->swap_chain_->recreate();
////        create_pipeline();
////        create_command_buffers();
//        recreate_swap_chain();
//        this->window_.set_framebuffer_resized(false);
//    } else if (res != VK_SUCCESS) {
//        throw runtime_error("failed to acquire swap chain image!");
//    }
//}

//void app::recreate_swap_chain()
//{
//    auto extent = this->window_.get_extent();
//
//    while (extent.width == 0 || extent.height == 0) {
//        extent = this->window_.get_extent();
//        glfwWaitEvents();
//    }
//
//    vkDeviceWaitIdle(this->device_.dev());
//
//    if (!this->swap_chain_)
//        this->swap_chain_ = std::make_unique<swap_chain>(this->device_, extent);
//    else {
//        this->swap_chain_ = std::make_unique<swap_chain>(this->device_, extent, std::move(this->swap_chain_));
//
//        if (this->command_buffers.size() != this->swap_chain_->imageCount()) {
//            free_command_buffers();
//            create_command_buffers();
//        }
//    }
//
//    create_pipeline();
//}

//void app::record_command_buffer(int image_index, double dt)
//{
//    VkCommandBufferBeginInfo begin_info{};
//
//    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//
//    if (vkBeginCommandBuffer(this->command_buffers[image_index], &begin_info) != VK_SUCCESS) {
//        throw runtime_error("failed to begin recording command buffer!");
//    }
//
//    VkRenderPassBeginInfo render_pass_info{};
//
//    VkRect2D scissor{{0, 0}, this->swap_chain_->getSwapChainExtent()};
//    VkViewport viewport{};
//
//    viewport.x = 0.f;
//    viewport.y = 0.f;
//    viewport.width = static_cast<float>(this->swap_chain_->getSwapChainExtent().width);
//    viewport.height = static_cast<float>(this->swap_chain_->getSwapChainExtent().height);
//    viewport.minDepth = 0.f;
//    viewport.maxDepth = 1.f;
//
//    vkCmdSetViewport(command_buffers[image_index], 0, 1, &viewport);
//    vkCmdSetScissor(command_buffers[image_index], 0, 1, &scissor);
//
//    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//    render_pass_info.renderPass = this->swap_chain_->getRenderPass();
//    render_pass_info.framebuffer = this->swap_chain_->getFrameBuffer(image_index);
//    render_pass_info.renderArea.offset = {0, 0};
//    render_pass_info.renderArea.extent = this->swap_chain_->getSwapChainExtent();
//
//    std::array<VkClearValue, 2> clear_vals{};
//
//    clear_vals[0].color = {0.01f, 0.01f, 0.01f, 1.f};
//    clear_vals[1].depthStencil = {1.f, 0};
//
//    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_vals.size());
//    render_pass_info.pClearValues = clear_vals.data();
//
//    vkCmdBeginRenderPass(command_buffers[image_index], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
//
//    this->pipeline_->bind(command_buffers[image_index]);
////        vkCmdDraw(command_buffers[image_index], 3, 1, 0, 0);
//
//    render_game_objects(command_buffers[image_index]);
//
//    vkCmdEndRenderPass(command_buffers[image_index]);
//    if (vkEndCommandBuffer(command_buffers[image_index]) != VK_SUCCESS) {
//        throw runtime_error("failed to record command buffer!");
//    }
//}
//
//void app::render_game_objects(VkCommandBuffer command_buffer)
//{
////    auto i = 0;
////    for (auto &obj: game_objects) {
////        i += 1;
////        glm::mod<float>(obj.transform_2d_.rotation + 0.001f * i, 2.f * glm::pi<float>());
////    }
//
//    this->pipeline_->bind(command_buffer);
//
//    for (auto &obj: this->game_objects) {
////        obj.transform_2d_.rotation = glm::mod(obj.transform_2d_.rotation + 0.001f, glm::two_pi<float>());
//
//        push_constant_data push{};
//
//        push.offset = obj.transform_2d_.translation;
//        push.color = obj.color;
//        push.transform = obj.transform_2d_.mat2();
//
//        vkCmdPushConstants(
//            command_buffer,
//            pipeline_layout,
//            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
//            0,
//            sizeof(push_constant_data),
//            &push);
//
//        obj.model_->bind(command_buffer);
//        obj.model_->draw(command_buffer);
//    }
//}
