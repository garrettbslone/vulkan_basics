//
// Created by Garrett on 12/26/2021.
//

#include "model.hpp"

#include <cassert>
#include <cstring>

std::vector<VkVertexInputBindingDescription> model::vertex::get_binding_descriptions()
{
    std::vector<VkVertexInputBindingDescription> binding_descriptions(1);

    binding_descriptions[0].binding = 0;
    binding_descriptions[0].stride = sizeof(vertex);
    binding_descriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return binding_descriptions;
}

std::vector<VkVertexInputAttributeDescription> model::vertex::get_attribute_descriptions()
{
    std::vector<VkVertexInputAttributeDescription> attribute_descriptions(2);

    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[0].offset = offsetof(vertex, position);

    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(vertex, color);

    return attribute_descriptions;
}

model::model(device *dev, const builder &b)
    : device_(dev)
{
    create_vertex_buffers(b.vertices);
    create_index_buffers(b.indices);
}

model::~model()
{
    vkDestroyBuffer(this->device_->dev(), vertex_buffer, nullptr);
    vkFreeMemory(this->device_->dev(), vertex_buffer_memory, nullptr);

    if (has_index_buffer) {
        vkDestroyBuffer(this->device_->dev(), index_buffer, nullptr);
        vkFreeMemory(this->device_->dev(), index_buffer_memory, nullptr);
    }
}

void model::bind(VkCommandBuffer command_buffer)
{
    VkBuffer buffers[] = {vertex_buffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(command_buffer, 0, 1, buffers, offsets);

    if (has_index_buffer)
        vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT32);
}

void model::draw(VkCommandBuffer command_buffer)
{
    if (has_index_buffer) {
        vkCmdDrawIndexed(command_buffer, index_count, 1, 0, 0, 0);
    } else {
        vkCmdDraw(command_buffer, vertex_count, 1, 0, 0);
    }
}

void model::create_vertex_buffers(const std::vector <vertex> &vertices)
{
    vertex_count = static_cast<uint32_t>(vertices.size());
    assert(vertex_count >= 3 && "Vertex count must be at least 3!");

    VkDeviceSize buffer_size = sizeof(vertices[0]) * vertex_count;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    this->device_->createBuffer(
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_buffer,
        staging_buffer_memory);

    void *data;
    vkMapMemory(this->device_->dev(), staging_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
    vkUnmapMemory(this->device_->dev(), staging_buffer_memory);

    this->device_->createBuffer(
        buffer_size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertex_buffer,
        vertex_buffer_memory);

    this->device_->copyBuffer(staging_buffer, vertex_buffer, buffer_size);

    vkDestroyBuffer(this->device_->dev(), staging_buffer, nullptr);
    vkFreeMemory(this->device_->dev(), staging_buffer_memory, nullptr);
}

void model::create_index_buffers(const std::vector<uint32_t> &indices)
{
    index_count = static_cast<uint32_t>(indices.size());
    has_index_buffer = index_count > 0;

    if (!has_index_buffer) return;

    VkDeviceSize buffer_size = sizeof(indices[0]) * index_count;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    this->device_->createBuffer(
        buffer_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_buffer,
        staging_buffer_memory);

    void *data;
    vkMapMemory(this->device_->dev(), staging_buffer_memory, 0, buffer_size, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
    vkUnmapMemory(this->device_->dev(), staging_buffer_memory);

    this->device_->createBuffer(
        buffer_size,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        index_buffer,
        index_buffer_memory);

    this->device_->copyBuffer(staging_buffer, index_buffer, buffer_size);

    vkDestroyBuffer(this->device_->dev(), staging_buffer, nullptr);
    vkFreeMemory(this->device_->dev(), staging_buffer_memory, nullptr);
}