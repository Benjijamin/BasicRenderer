#include <vkhelpers.h>
#include <vkstructs.h>
#include <stdexcept>

uint32_t findMemoryType(VkPhysicalDevice &physicalDevice, 
    uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if((typeFilter & (1 << i)) && 
        (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type");  
}

void createBuffer(VkDevice &device, VkPhysicalDevice &physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, 
    VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    populateBufferCreateInfo(bufferInfo, size, usage);

    if(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer))
    { throw std::runtime_error("failed to create buffer"); }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, 
        memoryRequirements.memoryTypeBits, properties);

    if(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory))
    { throw std::runtime_error("failed to allocate buffer memory"); }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void createImage(VkDevice &device, VkPhysicalDevice &physicalDevice, 
    uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
    VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
    VkImage &image, VkDeviceMemory &imageMemory)
{
    VkImageCreateInfo imageInfo{};
    populateImageCreateInfo(imageInfo, width, height, format, tiling, usage);

    if(vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
    { throw std::runtime_error("failed to create image"); }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, 
        memRequirements.memoryTypeBits, properties);
    
    if(vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    { throw std::runtime_error("failred to allocate image memory"); }

    vkBindImageMemory(device, image, imageMemory, 0);
}

VkImageView createImageView(VkDevice &device, VkImage &image, VkFormat format, 
    VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo{};
    populateImageViewCreateInfo(viewInfo, image, format, aspectFlags);

    VkImageView imageView;
    if(vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    { throw std::runtime_error("failed to create image view"); }

    return imageView;
}