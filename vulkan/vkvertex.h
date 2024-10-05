#ifndef VK_VERTEX_H
#define VK_VERTEX_H
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <array>
#include <glm.hpp>

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription();

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
};


#endif