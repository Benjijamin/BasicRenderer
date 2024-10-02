#ifndef VKDEBUG_H
#define VKDEBUG_H
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <vector>

bool checkValidationLayerSupport(const std::vector<const char*> & validationLayers);

void DestroyDebugUtilsMessengerEXT( 
    VkInstance instance, 
    VkDebugUtilsMessengerEXT debugMessenger, 
    const VkAllocationCallbacks* pAllocator);

void setupDebugMessenger(VkInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger);

#endif