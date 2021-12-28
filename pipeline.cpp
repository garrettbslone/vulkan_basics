//
// Created by Garrett on 12/24/2021.
//

#include "pipeline.hpp"
#include "model.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>

pipeline::pipeline(device *dev, const pipeline_config_info &info, const string &vert_path, const string &frag_path)
{
    this->dev_ = dev;
    create_pipeline(info, vert_path, frag_path);
}

vector<char> pipeline::read_file(const string &path)
{
    ifstream fin{path, ios::ate | ios::binary};

    if (!fin.is_open()) {
        throw runtime_error("Failed to open file: " + path);
    }

    size_t size = static_cast<size_t>(fin.tellg());
    vector<char> buff(size);

    fin.seekg(0);
    fin.read(buff.data(), size);
    fin.close();

    return buff;
}

pipeline::~pipeline()
{
    vkDestroyShaderModule(this->dev_->dev(), this->vert_sm, nullptr);
    vkDestroyShaderModule(this->dev_->dev(), this->frag_sm, nullptr);
    vkDestroyPipeline(this->dev_->dev(), this->vk_pipeline_, nullptr);
}

void pipeline::create_pipeline(const pipeline_config_info &info, const string &vert_path, const string &frag_path)
{
    auto vert = read_file(vert_path);
    auto frag = read_file(frag_path);

    create_shader_module(vert, &this->vert_sm);
    create_shader_module(frag, &this->frag_sm);

    VkPipelineShaderStageCreateInfo shader_stages[2]{};

    shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[0].flags = 0;
    shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stages[0].module = this->vert_sm;
    shader_stages[0].pName = "main";

    shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages[1].flags = 0;
    shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stages[1].module = this->frag_sm;
    shader_stages[1].pName = "main";

    auto binding_descriptions = model::vertex::get_binding_descriptions();
    auto attribute_descriptions = model::vertex::get_attribute_descriptions();
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};

    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_descriptions.size());
    vertex_input_info.pVertexBindingDescriptions = binding_descriptions.data();
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
    vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

    VkGraphicsPipelineCreateInfo pipeline_info{};

    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &info.input_assembly_info;
    pipeline_info.pViewportState = &info.viewport_info;
    pipeline_info.pRasterizationState = &info.raster_info;
    pipeline_info.pMultisampleState = &info.multisample_info;
    pipeline_info.pDepthStencilState = &info.depth_stencil_info;
    pipeline_info.pColorBlendState = &info.color_blend_info;
    pipeline_info.pDynamicState = &info.dynamic_state_info;
    pipeline_info.layout = info.pipeline_layout;
    pipeline_info.renderPass = info.render_pass;
    pipeline_info.subpass = info.subpass;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(this->dev_->dev(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->vk_pipeline_) != VK_SUCCESS) {
        throw runtime_error("failed to create graphics pipeline");
    }
}

void pipeline::default_config_info(pipeline_config_info &info)
{
    info.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    info.viewport_info.viewportCount = 1;
    info.viewport_info.pViewports = nullptr;
    info.viewport_info.scissorCount = 1;
    info.viewport_info.pScissors = nullptr;

    info.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    info.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    info.input_assembly_info.primitiveRestartEnable = VK_FALSE;

    info.raster_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    info.raster_info.depthClampEnable = VK_FALSE;
    info.raster_info.rasterizerDiscardEnable = VK_FALSE;
    info.raster_info.polygonMode = VK_POLYGON_MODE_FILL;
    info.raster_info.cullMode = VK_CULL_MODE_NONE;
    info.raster_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    info.raster_info.depthBiasEnable = VK_FALSE;
    info.raster_info.depthBiasConstantFactor = 0.f;
    info.raster_info.depthBiasClamp = 0.f;
    info.raster_info.depthBiasSlopeFactor = 0.f;
    info.raster_info.lineWidth = 1.f;

    info.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    info.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    info.multisample_info.sampleShadingEnable = VK_FALSE;
    info.multisample_info.minSampleShading = 1.f;
    info.multisample_info.pSampleMask = nullptr;
    info.multisample_info.alphaToCoverageEnable = VK_FALSE;
    info.multisample_info.alphaToOneEnable = VK_FALSE;

    info.color_blend_attachment.blendEnable = VK_FALSE;
    info.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    info.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    info.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    info.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    info.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    info.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
    info.color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    info.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    info.color_blend_info.logicOpEnable = VK_FALSE;
    info.color_blend_info.logicOp = VK_LOGIC_OP_COPY;
    info.color_blend_info.attachmentCount = 1;
    info.color_blend_info.pAttachments = &info.color_blend_attachment;
    info.color_blend_info.blendConstants[0] = 0.f;
    info.color_blend_info.blendConstants[1] = 0.f;
    info.color_blend_info.blendConstants[2] = 0.f;
    info.color_blend_info.blendConstants[3] = 0.f;

    info.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.depth_stencil_info.depthTestEnable = VK_TRUE;
    info.depth_stencil_info.depthWriteEnable = VK_TRUE;
    info.depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
    info.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    info.depth_stencil_info.stencilTestEnable = VK_FALSE;
    info.depth_stencil_info.front = {};
    info.depth_stencil_info.back = {};
    info.depth_stencil_info.minDepthBounds = 0.f;
    info.depth_stencil_info.maxDepthBounds = 1.f;

    info.dynamic_state_enables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    info.dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    info.dynamic_state_info.pDynamicStates = info.dynamic_state_enables.data();
    info.dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(info.dynamic_state_enables.size());
    info.dynamic_state_info.flags = 0;
}

void pipeline::create_shader_module(const vector<char> &code, VkShaderModule *sm)
{
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

    if (vkCreateShaderModule(this->dev_->dev(), &create_info, nullptr, sm) != VK_SUCCESS) {
        throw runtime_error("failed to create shader module");
    }
}

void pipeline::bind(VkCommandBuffer buffer)
{
    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->vk_pipeline_);
}
