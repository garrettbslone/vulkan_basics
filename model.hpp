//
// Created by Garrett on 12/26/2021.
//

#ifndef VK_BASICS_MODEL_HPP
#define VK_BASICS_MODEL_HPP

#include "device.hpp"
#include "buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>

class model {
public:
    struct vertex {
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{};

        static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
        static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();

        inline bool operator==(const vertex &other) const
        {
            return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
        }
    };

    struct builder {
        std::vector<vertex> vertices{};
        std::vector<uint32_t> indices{};

        void load_model(const std::string &path);
    };

    model(device *dev, const builder &b);
    virtual ~model();

    model(const model &) = delete;
    model &operator=(const model &) = delete;

    static std::unique_ptr<model> create_model_from_file(device &d, const std::string &path);

    void bind(VkCommandBuffer command_buffer);
    void draw(VkCommandBuffer command_buffer);

private:
    void create_vertex_buffers(const std::vector<vertex> &vertices);
    void create_index_buffers(const std::vector<uint32_t> &indices);

    device *device_;
    std::unique_ptr<buffer> vertex_buffer;
    uint32_t vertex_count;

    bool has_index_buffer = false;
    std::unique_ptr<buffer> index_buffer;
    uint32_t index_count;
};

#endif //VK_BASICS_MODEL_HPP
