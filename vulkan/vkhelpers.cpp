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