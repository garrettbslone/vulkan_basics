//
// Created by Garrett on 12/26/2021.
//

#ifndef VK_BASICS_MODEL_HPP
#define VK_BASICS_MODEL_HPP

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

class model {
public:
    struct vertex {
        glm::vec2 position;
        glm::vec3 color;

        static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
        static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();
    };

    model(device *dev, const std::vector<vertex> &vertices);
    virtual ~model();

    model(const model &) = delete;
    model &operator=(const model &) = delete;

    void bind(VkCommandBuffer command_buffer);
    void draw(VkCommandBuffer command_buffer);

private:
    void create_vertex_buffers(const std::vector<vertex> &vertices);

    device *device_;
    VkBuffer vertex_buffer;
    VkDeviceMemory vertex_buffer_memory;
    uint32_t vertex_count;
};

#endif //VK_BASICS_MODEL_HPP
