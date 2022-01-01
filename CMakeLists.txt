cmake_minimum_required(VERSION 3.20)
project(vk_basics)

set(CMAKE_CXX_STANDARD 17)

add_executable(vk_basics main.cpp app.cpp app.hpp window.cpp window.hpp pipeline.cpp pipeline.hpp device.cpp device.hpp swap_chain.cpp swap_chain.hpp model.cpp model.hpp game_object.cpp game_object.hpp renderer.cpp renderer.hpp render_system.cpp render_system.hpp camera.cpp camera.hpp keyboard_movement_controller.cpp keyboard_movement_controller.hpp utils.hpp buffer.cpp buffer.hpp frame_info.hpp descriptors.cpp descriptors.hpp)

add_subdirectory(glfw-3.3.6/glfw-3.3.6)

# 1. Set VULKAN_SDK_PATH in .env.cmake to target specific vulkan version
if (DEFINED VULKAN_SDK_PATH)
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
    message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

target_include_directories(vk_basics PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(vk_basics PUBLIC glfw-3.3.6/glfw-3.3.6/include)
target_include_directories(vk_basics PUBLIC glm/glm-0.9.9.8)
target_include_directories(vk_basics PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vk_basics PRIVATE glfw)
target_link_libraries(vk_basics PUBLIC ${Vulkan_LIBRARIES})

make_directory(${CMAKE_BINARY_DIR}/shaders)
file(GLOB SHADERS_SPV ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.spv)
file(COPY ${SHADERS_SPV} DESTINATION ${CMAKE_BINARY_DIR}/shaders)
file(GLOB MODEL_FILES ${CMAKE_SOURCE_DIR}/models/*)
file(COPY ${MODEL_FILES} DESTINATION ${CMAKE_BINARY_DIR}/models)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.frag.spv DESTINATION ${CMAKE_BINARY_DIR}/shaders)
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.vert.spv DESTINATION ${CMAKE_BINARY_DIR}/shaders)