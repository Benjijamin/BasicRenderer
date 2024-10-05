#ifndef VK_HELPERS_H
#define VK_HELPERS_H
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN

uint32_t findMemoryType(VkPhysicalDevice &physicalDevice, 
    uint32_t typeFilter, VkMemoryPropertyFlags properties);

void createBuffer(VkDevice &device, VkPhysicalDevice &physicalDevice, 
    VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
    VkBuffer &buffer, VkDeviceMemory &bufferMemory);

#endif