//
// Created by Garrett on 12/24/2021.
//

#ifndef VK_BASICS_APP_HPP
#define VK_BASICS_APP_HPP

#include "camera.hpp"
#include "window.hpp"
#include "model.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "descriptors.hpp"

#include <memory>

class app {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    app();
    ~app();

    app(const app &) = delete;
    app &operator=(const app &) = delete;

    void run();

private:
    void load_game_objects();

    window window_{WIDTH, HEIGHT, "Hello Vulkan!"};
    device device_{window_};
    renderer renderer_{window_, device_};

    unique_ptr<descriptor_pool> global_pool{};
    game_object::Map game_objects;
};

#endif //VK_BASICS_APP_HPP
