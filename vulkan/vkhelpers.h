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

void createImage(VkDevice &device, VkPhysicalDevice &physicalDevice, 
    uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
    VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
    VkImage &image, VkDeviceMemory &imageMemory);

VkImageView createImageView(VkDevice &device, VkImage &image, VkFormat format, 
    VkImageAspectFlags aspectFlags);

#endif