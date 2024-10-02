#ifndef VK_STRUCTS_H
#define VK_STRUCTS_H
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <vector>

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

void populateAppInfo(VkApplicationInfo &appInfo);

void populateQueueCreateInfo(VkDeviceQueueCreateInfo &createInfo, 
    const uint32_t &queueFamily, const float &queuePriority);

void populateDeviceCreateInfo(VkDeviceCreateInfo &createInfo, 
    const std::vector<VkDeviceQueueCreateInfo> &queueCreateInfos,
    const VkPhysicalDeviceFeatures &deviceFeatures,
    const std::vector<const char*> &deviceExtensions,
    const bool &enableValidationLayers,
    const std::vector<const char*> &validationLayers);

void populateSwapchainCreateInfo(VkSwapchainCreateInfoKHR &createInfo, 
    const std::vector<VkSurfaceFormatKHR> &formats,
    const std::vector<VkPresentModeKHR> &presentModes,
    const VkSurfaceCapabilitiesKHR &capabilities,
    const int &width, const int &height, const uint32_t &imageCount,
    const VkSurfaceKHR &surface, uint32_t* queueFamilyIndices,
    VkFormat &outFormat, VkExtent2D &outExtent);

void populateImageViewCreateInfo(VkImageViewCreateInfo &createInfo,
    VkImage &image, VkFormat &imageFormat);

void populateColorAttachment(VkAttachmentDescription &attachment, VkFormat &imageFormat);

void populateColorAttachmentRef(VkAttachmentReference &colorAttachmentRef);

void populateSubpass(VkSubpassDescription &subpass, VkAttachmentReference &colorAttachmentRef);

void populateSubpassDependency(VkSubpassDependency &dependency);

void populateRenderPassCreateInfo(VkRenderPassCreateInfo &createInfo,
    VkAttachmentDescription &colorAttachment, VkSubpassDescription &subpass,
    VkSubpassDependency &dependency);

void populateVertShaderStageCreateInfo(VkPipelineShaderStageCreateInfo &vertShaderStageInfo,
    VkShaderModule &vertShaderModule);

void populateFragShaderStageCreateInfo(VkPipelineShaderStageCreateInfo &fragShaderStageInfo,
    VkShaderModule &fragShaderModule);

void populatePipelineDynamicStateCreateInfo(VkPipelineDynamicStateCreateInfo &dynamicState,
    std::vector<VkDynamicState> &dynamicStates);

void populatePipelineVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo &vertexInputInfo);

void populatePipelineInputAssemblyStateCreateInfo(VkPipelineInputAssemblyStateCreateInfo &inputAssembly);

void populateViewport(VkViewport &viewport, VkExtent2D &swapChainExtent);

void populateScissor(VkRect2D &scissor, VkExtent2D &swapChainExtent);

void populatePipelineViewportStateCreateInfo(VkPipelineViewportStateCreateInfo &viewportState,
    VkViewport &viewport, VkRect2D &scissor);

void populatePipelineRasterizationStateCreateInfo(VkPipelineRasterizationStateCreateInfo &rasterizer);

void populatePipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo &multisampling);

void populatePipelineColorBlendAttachmentState(VkPipelineColorBlendAttachmentState &colorBlendAttachment);

void populatePipelineColorBlendStateCreateInfo(VkPipelineColorBlendStateCreateInfo &colorBlending,
    VkPipelineColorBlendAttachmentState &colorBlendAttachment);

void populatePipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo &pipelineLayoutInfo);

#endif