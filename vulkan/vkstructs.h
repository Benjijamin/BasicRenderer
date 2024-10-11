#ifndef VK_STRUCTS_H
#define VK_STRUCTS_H
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <vector>
#include <vkvertex.h>
#include <array>

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
    VkImage &image, VkFormat &imageFormat, VkImageAspectFlags &aspectFlags);

void populateColorAttachment(VkAttachmentDescription &attachment, VkFormat &imageFormat);

void populateColorAttachmentRef(VkAttachmentReference &colorAttachmentRef);

void populateSubpass(VkSubpassDescription &subpass, 
    VkAttachmentReference &colorAttachmentRef, VkAttachmentReference &depthAttachmentRef);

void populateSubpassDependency(VkSubpassDependency &dependency);

void populateRenderPassCreateInfo(VkRenderPassCreateInfo &createInfo,
    std::array<VkAttachmentDescription, 2> &attachments, VkSubpassDescription &subpass,
    VkSubpassDependency &dependency);

void populateVertShaderStageCreateInfo(VkPipelineShaderStageCreateInfo &vertShaderStageInfo,
    VkShaderModule &vertShaderModule);

void populateFragShaderStageCreateInfo(VkPipelineShaderStageCreateInfo &fragShaderStageInfo,
    VkShaderModule &fragShaderModule);

void populatePipelineDynamicStateCreateInfo(VkPipelineDynamicStateCreateInfo &dynamicState,
    std::vector<VkDynamicState> &dynamicStates);

void populatePipelineVertexInputStateCreateInfo(VkPipelineVertexInputStateCreateInfo &vertexInputInfo,
    VkVertexInputBindingDescription &vertexBindingDescription, 
    std::array<VkVertexInputAttributeDescription, 3> &vertexAttributeDescriptions);

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

void populatePipelineLayoutCreateInfo(VkPipelineLayoutCreateInfo &pipelineLayoutInfo,
    VkDescriptorSetLayout &descriptorSetLayout);

void populateVertexBufferCreateInfo(VkBufferCreateInfo &bufferInfo,
    const std::vector<Vertex> &vertices);

void populateBufferCreateInfo(VkBufferCreateInfo &bufferInfo, VkDeviceSize &size,
    VkBufferUsageFlags &usage);

void populateUniformBufferObjectLayoutBinding(VkDescriptorSetLayoutBinding &uboLayoutBinding);

void populateDescriptorSetLayoutCreateInfo(VkDescriptorSetLayoutCreateInfo &layoutInfo,
    std::array<VkDescriptorSetLayoutBinding, 2> &layoutBindings);

void populateWriteDescriptorSet(std::array<VkWriteDescriptorSet, 2> &descriptorWrites, 
    VkDescriptorSet &descriptorSet, VkDescriptorBufferInfo &bufferInfo, VkDescriptorImageInfo &imageInfo);

void populateImageCreateInfo(VkImageCreateInfo &imageInfo, uint32_t width, uint32_t height,
    VkFormat &format, VkImageTiling &tiling, VkImageUsageFlags &usage);

void populateImageMemoryBarrier(VkImageMemoryBarrier &barrier,
    VkImageLayout &oldLayout, VkImageLayout &newLayout, VkImage &image);

void populateSamplerCreateInfo(VkSamplerCreateInfo &samplerInfo, float maxAnisotropy);

void populateDepthAttachment(VkAttachmentDescription &depthAttachment, VkFormat format);

void populateDepthAttachmentRef(VkAttachmentReference &depthAttachmentRef);

void populatePipelineDepthStencilStateCreateInfo(VkPipelineDepthStencilStateCreateInfo &depthStencil);

#endif