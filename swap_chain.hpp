//
// Created by Garrett on 12/24/2021.
//

#ifndef VK_BASICS_SWAP_CHAIN_HPP
#define VK_BASICS_SWAP_CHAIN_HPP

#include "device.hpp"

#include <memory>
#include <string>
#include <vector>

class swap_chain {
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    swap_chain(device &deviceRef, VkExtent2D windowExtent);
    swap_chain(device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<swap_chain> previous);
    ~swap_chain();

    swap_chain(const swap_chain &) = delete;
    swap_chain &operator=(const swap_chain &) = delete;

    VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
    VkRenderPass getRenderPass() { return renderPass; }
    VkImageView getImageView(int index) { return swapChainImageViews[index]; }
    size_t imageCount() { return swapChainImages.size(); }
    VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
    VkExtent2D getSwapChainExtent() { return swapChainExtent; }
    uint32_t width() { return swapChainExtent.width; }
    uint32_t height() { return swapChainExtent.height; }

    float extentAspectRatio() {
        return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
    }
    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t *imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

    void recreate();

    bool compare_swap_formats(const swap_chain &chain) const
    {
        return chain.swapChainImageFormat == this->swapChainImageFormat &&
               chain.swap_chain_depth_format == this->swap_chain_depth_format;
    }

private:
    void init();
    void cleanup_swap_chain();

    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    VkFormat swapChainImageFormat;
    VkFormat swap_chain_depth_format;
    VkExtent2D swapChainExtent;

    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkRenderPass renderPass;

    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthImageMemorys;
    std::vector<VkImageView> depthImageViews;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    device &device_;
    VkExtent2D windowExtent;

    VkSwapchainKHR swapChain;
    std::shared_ptr<swap_chain> old_swap_chain;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;
};

#endif //VK_BASICS_SWAP_CHAIN_HPP
