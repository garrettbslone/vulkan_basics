//
// Created by Garrett on 12/29/2021.
//

#ifndef VK_BASICS_FRAME_INFO_HPP
#define VK_BASICS_FRAME_INFO_HPP

#include "camera.hpp"

#include <vulkan/vulkan.h>

struct frame_info {
    int frame_index;
    float frame_time;
    VkCommandBuffer command_buffer;
    camera &c;
};

#endif //VK_BASICS_FRAME_INFO_HPP
