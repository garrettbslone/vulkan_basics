//
// Created by Garrett on 12/29/2021.
//

#ifndef VK_BASICS_FRAME_INFO_HPP
#define VK_BASICS_FRAME_INFO_HPP

#include "camera.hpp"
#include "game_object.hpp"

#include <vulkan/vulkan.h>

struct frame_info {
    int frame_index;
    float frame_time;
    VkCommandBuffer command_buffer;
    camera &c;
    VkDescriptorSet global_desctiptor_set;
    game_object::Map &game_objects;
};

#endif //VK_BASICS_FRAME_INFO_HPP
