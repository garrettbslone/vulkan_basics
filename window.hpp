//
// Created by Garrett on 12/24/2021.
//

#ifndef VK_BASICS_WINDOW_HPP
#define VK_BASICS_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class window {
public:
    window(int width, int height, const std::string &name);
    virtual ~window();

    window(const window &) = delete;
    window &operator=(const window &) = delete;

    inline bool close() { return glfwWindowShouldClose(this->w); }
    inline VkExtent2D get_extent() { return { static_cast<uint32_t>(this->width), static_cast<uint32_t>(this->height)}; }
    void set_extent(int new_width, int new_height);

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    bool is_framebuffer_resized() const;
    void set_framebuffer_resized(bool framebuffer_resized);

    GLFWwindow *get_GLFWWindow() const { return this->w; }

private:
    void init();

    int width;
    int height;

    std::string name;
    GLFWwindow *w;

    bool framebuffer_resized = false;
};

#endif //VK_BASICS_WINDOW_HPP
