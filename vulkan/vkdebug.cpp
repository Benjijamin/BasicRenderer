#include <vkdebug.h>
#include <vkstructs.h>
#include <string.h>
#include <iostream>

bool checkValidationLayerSupport(const std::vector<const char*> &validationLayers)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for(const char* layerName : validationLayers) 
    {
        bool layerFound = false;

        for(const auto& layerProperties : availableLayers)
        {
            if(strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
        { return false; }
    }

    return true;
}

VkResult CreateDebugUtilsMessengerEXT( 
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, 
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(func != nullptr) 
    { 
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger); 
    }
    else
    { 
        return VK_ERROR_EXTENSION_NOT_PRESENT; 
    }
}

void DestroyDebugUtilsMessengerEXT( 
    VkInstance instance, 
    VkDebugUtilsMessengerEXT debugMessenger, 
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr)
    { 
        func(instance, debugMessenger, pAllocator); 
    }
}

void setupDebugMessenger(VkInstance &instance, VkDebugUtilsMessengerEXT &debugMessenger)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    std::cout << "Creating debug utils..." << std::endl;
    if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
    { throw std::runtime_error("failed to set up debug messenger"); }
}