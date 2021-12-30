//
// Created by Garrett on 12/26/2021.
//

#include "model.hpp"
#include "utils.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>
#include <cassert>
#include <cstring>

namespace std {
template<>
struct hash<model::vertex> {
    size_t operator()(model::vertex const &v) const
    {
        size_t seed = 0;
        hashCombine(seed, v.position, v.color, v.normal, v.uv);
        return seed;
    }
};
}

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
    std::vector<VkVertexInputAttributeDescription> attribute_descriptions{};

    attribute_descriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, position)});
    attribute_descriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, color)});
    attribute_descriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, normal)});
    attribute_descriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex, uv)});

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

std::unique_ptr<model> model::create_model_from_file(device &d, const std::string &path)
{
    builder b{};
    b.load_model(path);
    return std::make_unique<model>(&d, b);
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

void model::builder::load_model(const std::string &path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        throw std::runtime_error(warn + err);
    }

    vertices.clear();
    indices.clear();

    std::unordered_map<vertex, uint32_t> unique_vertices{};

    for (const auto &shape: shapes) {
        for (const auto &index: shape.mesh.indices) {
            vertex v{};

            if (index.vertex_index >= 0) {
                v.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                v.color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2]
                };
            }

            if (index.normal_index >= 0) {
                v.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }

            if (index.texcoord_index >= 0) {
                v.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }

            if (unique_vertices.count(v) == 0) {
                unique_vertices[v] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(v);
            }

            indices.push_back(unique_vertices[v]);
        }
    }
}
