//
// Created by Garrett on 12/24/2021.
//

#ifndef VK_BASICS_PIPELINE_HPP
#define VK_BASICS_PIPELINE_HPP

#include <string>
#include <vector>

#include "device.hpp"

using namespace std;

struct pipeline_config_info {
    pipeline_config_info(const pipeline_config_info &) = delete;
    pipeline_config_info &operator=(const pipeline_config_info &) = delete;

    VkPipelineViewportStateCreateInfo viewport_info;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
    VkPipelineRasterizationStateCreateInfo raster_info;
    VkPipelineMultisampleStateCreateInfo multisample_info;
    VkPipelineColorBlendAttachmentState color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo color_blend_info;
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
    vector<VkDynamicState> dynamic_state_enables;
    VkPipelineDynamicStateCreateInfo dynamic_state_info;
    VkPipelineLayout pipeline_layout = nullptr;
    VkRenderPass render_pass = nullptr;
    uint32_t subpass = 0;
};

class pipeline {
public:
    pipeline(device *dev, const pipeline_config_info &info, const string &vert_path, const string &frag_path);
    virtual ~pipeline();

    pipeline(const pipeline &p) = delete;
    pipeline &operator=(const pipeline &p) = delete;

    static void default_config_info(pipeline_config_info &info);

    void bind(VkCommandBuffer buffer);

private:
    static vector<char> read_file(const string &path);
    void create_pipeline(const pipeline_config_info &info, const string &vert_path, const string &frag_path);

    void create_shader_module(const vector<char> &code, VkShaderModule *sh);

    device *dev_;
    VkPipeline vk_pipeline_;
    VkShaderModule vert_sm;
    VkShaderModule frag_sm;
};

#endif //VK_BASICS_PIPELINE_HPP
