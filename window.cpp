//
// Created by Garrett on 12/24/2021.
//

#include "window.hpp"

#include <stdexcept>

window::window(int width, int height, const std::string &name) : width(width), height(height), name(name)
{
    init();
}

window::~window()
{
    glfwDestroyWindow(this->w);
    glfwTerminate();
}

void window::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    this->w = glfwCreateWindow(width, height, this->name.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(this->w, this);
    glfwSetFramebufferSizeCallback(this->w, [](GLFWwindow *w_, int width_, int height_)
    {
        auto win = reinterpret_cast<window *>(glfwGetWindowUserPointer(w_));
        win->set_framebuffer_resized(true);
        win->width = width_;
        win->height = height_;
    });
}

void window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
{
    if (glfwCreateWindowSurface(instance, this->w, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
}
bool window::is_framebuffer_resized() const
{
    return framebuffer_resized;
}

void window::set_framebuffer_resized(bool framebuffer_resized)
{
    this->framebuffer_resized = framebuffer_resized;
}
