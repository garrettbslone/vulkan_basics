//
// Created by Garrett on 12/26/2021.
//

#include "render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct push_constant_data {
    glm::mat4 model_matrix{1.f};
    glm::mat4 normal_matrix{1.f};
};

render_system::render_system(device &d, VkRenderPass render_pass, VkDescriptorSetLayout gloat_set_layout)
    : device_{d}
{
    create_pipeline_layout(gloat_set_layout);
    create_pipeline(render_pass);
}

render_system::~render_system()
{
    vkDestroyPipelineLayout(this->device_.dev(), this->pipeline_layout, nullptr);
}
void render_system::create_pipeline_layout(VkDescriptorSetLayout gloat_set_layout)
{
    VkPushConstantRange push_constant_range{};

    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    push_constant_range.offset = 0;
    push_constant_range.size = sizeof(push_constant_data);

    vector<VkDescriptorSetLayout> descriptor_set_layouts{gloat_set_layout};

    VkPipelineLayoutCreateInfo pipeline_layout_info{};

    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size());
    pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();
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

void render_system::render_game_objects(frame_info &info)
{
    this->pipeline_->bind(info.command_buffer);

    vkCmdBindDescriptorSets(
        info.command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline_layout,
        0,
        1,
        &info.global_desctiptor_set,
        0,
        nullptr);

    for (auto &[id, obj]: info.game_objects) {
        if (!obj.model_)
            continue;

        push_constant_data push{};

        push.model_matrix = obj.transform_.mat4();
        push.normal_matrix = obj.transform_.normal_matrix();

        vkCmdPushConstants(
            info.command_buffer,
            pipeline_layout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(push_constant_data),
            &push);

        obj.model_->bind(info.command_buffer);
        obj.model_->draw(info.command_buffer);
    }
}
