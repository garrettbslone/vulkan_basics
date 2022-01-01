//
// Created by Garrett on 12/29/2021.
//

#ifndef VK_BASICS_DESCRIPTORS_HPP
#define VK_BASICS_DESCRIPTORS_HPP

#include "device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

class descriptor_set_layout {
public:
    class Builder {
    public:
        Builder(device &d) : device_{d} {}

        Builder &addBinding(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stageFlags,
            uint32_t count = 1);
        std::unique_ptr<descriptor_set_layout> build() const;

    private:
        device &device_;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
    };

    descriptor_set_layout(
        device &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~descriptor_set_layout();
    descriptor_set_layout(const descriptor_set_layout &) = delete;
    descriptor_set_layout &operator=(const descriptor_set_layout &) = delete;

    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

private:
    device &device_;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

    friend class descriptor_writer;
};

class descriptor_pool {
public:
    class Builder {
    public:
        Builder(device &d) : device_{d} {}

        Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder &setMaxSets(uint32_t count);
        std::unique_ptr<descriptor_pool> build() const;

    private:
        device &device_;
        std::vector<VkDescriptorPoolSize> poolSizes{};
        uint32_t maxSets = 1000;
        VkDescriptorPoolCreateFlags poolFlags = 0;
    };

    descriptor_pool(
        device &device,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize> &poolSizes);
    ~descriptor_pool();
    descriptor_pool(const descriptor_pool &) = delete;
    descriptor_pool &operator=(const descriptor_pool &) = delete;

    bool allocateDescriptorSet(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

    void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

    void resetPool();

private:
    device &device_;
    VkDescriptorPool descriptorPool;

    friend class descriptor_writer;
};

class descriptor_writer {
public:
    descriptor_writer(descriptor_set_layout &setLayout, descriptor_pool &pool);

    descriptor_writer &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
    descriptor_writer &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

    bool build(VkDescriptorSet &set);
    void overwrite(VkDescriptorSet &set);

private:
    descriptor_set_layout &setLayout;
    descriptor_pool &pool;
    std::vector<VkWriteDescriptorSet> writes;
};

#endif //VK_BASICS_DESCRIPTORS_HPP
