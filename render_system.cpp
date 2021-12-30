//
// Created by Garrett on 12/26/2021.
//

#include "render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct push_constant_data {
    glm::mat4 transform{1.f};
    alignas(16) glm::vec3 color;
};

render_system::render_system(device &d, VkRenderPass render_pass)
    : device_{d}
{
    create_pipeline_layout();
    create_pipeline(render_pass);
}

render_system::~render_system()
{
    vkDestroyPipelineLayout(this->device_.dev(), this->pipeline_layout, nullptr);
}
void render_system::create_pipeline_layout()
{
    VkPushConstantRange push_constant_range{};

    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(push_constant_data);

    VkPipelineLayoutCreateInfo pipeline_layout_info{};

    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &push_constant_range;

    if (vkCreatePipelineLayout(this->device_.dev(), &pipeline_layout_info, nullptr, &this->pipeline_layout) != VK_SUCCESS) {
        throw runtime_error("failed to create pipeline layout!");
    }
}

void render_system::create_pipeline(VkRenderPass render_pass)
{
    pipeline_config_info pipeline_config{};
    pipeline::default_config_info(pipeline_config);

    pipeline_config.render_pass = render_pass;
    pipeline_config.pipeline_layout = this->pipeline_layout;

    this->pipeline_ = std::make_unique<pipeline>(
        &this->device_,
        pipeline_config,
        "shaders/shader.vert.spv",
        "shaders/shader.frag.spv"
    );
}

void render_system::render_game_objects(VkCommandBuffer command_buffer, vector<game_object> &objects, const camera &camera_)
{
    this->pipeline_->bind(command_buffer);

    auto projection_view = camera_.get_view() * camera_.get_projection();

    for (auto &obj: objects) {
//        obj.transform_.rotation.y = glm::mod(obj.transform_.rotation.y + 0.0001f, glm::two_pi<float>());
//        obj.transform_.rotation.x = glm::mod(obj.transform_.rotation.x + 0.00005f, glm::two_pi<float>());

        push_constant_data push{};

        push.color = obj.color;
        push.transform = projection_view * obj.transform_.mat4();

        vkCmdPushConstants(
            command_buffer,
            pipeline_layout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(push_constant_data),
            &push);

        obj.model_->bind(command_buffer);
        obj.model_->draw(command_buffer);
    }
}
