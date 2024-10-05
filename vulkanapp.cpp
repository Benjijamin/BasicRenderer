#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vkstructs.h>
#include <vkdebug.h>
#include <vkvertex.h>
#include <vkhelpers.h>
#include <utils.h>
#include <stdexcept>
#include <vector>
#include <set>
#include <array>
#include <iostream>
#include <string.h>
#include <optional>
#include <limits>
#include <algorithm>
#include <chrono>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails 
{
    VkSurfaceCapabilitiesKHR capabilites;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class VulkanApp
{
    public:
        void run()
        {
            initWindow();
            initVulkan();
            mainLoop();
            cleanup();
        }
        
    private:
        GLFWwindow *window;
        VkInstance instance;
        VkDevice device;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkSurfaceKHR surface;
        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;
        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkCommandPool commandPool;
        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBuffersMapped;

        VkDebugUtilsMessengerEXT debugMessenger;

        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        bool frambufferResized = false;

        const uint32_t WIDTH = 800;
        const uint32_t HEIGHT = 600;

        const int MAX_FRAMES_IN_FLIGHT = 2;
        uint32_t currentFrame = 0;

        const std::vector<Vertex> vertices = 
        {
            { {-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f} },
            { {0.5f, -0.5f}, {0.0f, 1.0f, 0.0f} },
            { {0.5f, 0.5f}, {0.0f, 0.0f, 1.0f} },
            { {-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f} }
        };

        const std::vector<uint16_t> indices = 
        {
            0, 1, 2, 2, 3, 0
        };

        const bool enableValidationLayers = true;
        const std::vector<const char*> validationLayers = 
        {
            "VK_LAYER_KHRONOS_validation"
        };

        const std::vector<const char*> deviceExtensions =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        
        std::vector<const char*> getRequiredExtensions()
        {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

            if(enableValidationLayers)
            { extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); }

            return extensions;
        }

        //-----------------------$Instance----------------------//
        //------------------------------------------------------//
        //------------------------------------------------------//
        
        void createInstance()
        {
            if(enableValidationLayers && !checkValidationLayerSupport(validationLayers))
            { throw std::runtime_error("validation layers not available"); }

            VkApplicationInfo appInfo{};
            populateAppInfo(appInfo);

            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;
            
            auto extensions = getRequiredExtensions();
            createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
            createInfo.ppEnabledExtensionNames = extensions.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            if (enableValidationLayers) 
            {
                createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                createInfo.ppEnabledLayerNames = validationLayers.data();
                populateDebugMessengerCreateInfo(debugCreateInfo);
                createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
            } else 
            {
                createInfo.enabledLayerCount = 0;
                createInfo.pNext = nullptr;
            }
           
            std::cout << "Creating vulkan instance..." << std::endl;
            if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) 
            { throw std::runtime_error("failed to create instance!"); }
        }

        void initWindow()
        {
            glfwInit();
            //no OpenGL
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            window = glfwCreateWindow(800, 600, "VulkanApp", nullptr, nullptr);
            glfwSetWindowUserPointer(window, this);
            glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        }

        static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
        {
            VulkanApp* app = reinterpret_cast<VulkanApp*>(glfwGetWindowUserPointer(window));
            app -> frambufferResized = true;
        }

        //-----------------------$Device----------------------//
        //----------------------------------------------------//
        //----------------------------------------------------//

        void pickPhysicalDevice()
        {
            uint32_t deviceCount = 0;
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

            if(deviceCount == 0)
            { throw std::runtime_error("no GPU found"); }

            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

            for(const auto& device : devices)
            {
                if(isDeviceSuitable(device))
                {
                    physicalDevice = device;
                    break;
                }
            }

            if(physicalDevice == VK_NULL_HANDLE)
            { throw std::runtime_error("no suitable GPU"); }
        }
        
        bool checkDeviceExtensionSupport(VkPhysicalDevice device)
        {
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data()); 

            std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

            for(const auto& extension : availableExtensions)
            {
                requiredExtensions.erase(extension.extensionName);
            }

            return requiredExtensions.empty();
        }

        void createLogicalDevice()
        {
            QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
            std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                indices.presentFamily.value()};

            float queuePriority = 1.0f;
            for(uint32_t queueFamily : uniqueQueueFamilies)
            {
                VkDeviceQueueCreateInfo queueCreateInfo{};
                populateQueueCreateInfo(queueCreateInfo,
                    queueFamily, queuePriority);

                queueCreateInfos.push_back(queueCreateInfo);
            }
            
            VkPhysicalDeviceFeatures deviceFeatures{};

            VkDeviceCreateInfo createInfo{};
            populateDeviceCreateInfo(createInfo, queueCreateInfos, deviceFeatures,
                deviceExtensions, enableValidationLayers, validationLayers);

            std::cout << "Creating logical device..." << std::endl;
            if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create logical device");
            }

            vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
            vkGetDeviceQueue(device, indices.presentFamily.value(), 0 ,&presentQueue);
        }

        bool isDeviceSuitable(VkPhysicalDevice device)
        {
            QueueFamilyIndices indices = findQueueFamilies(device);

            bool extensionsSupported = checkDeviceExtensionSupport(device);

            bool swapChainAdequate = false;
            if(extensionsSupported)
            {
                SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
                swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
            }

            return indices.isComplete() && extensionsSupported && swapChainAdequate;
        }

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
        {
            QueueFamilyIndices indices;

            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

            int i = 0;
            for(const auto& queueFamily : queueFamilies)
            {
                if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                { indices.graphicsFamily = i; }

                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

                if(presentSupport)
                { indices.presentFamily = i; }

                if(indices.isComplete()) break;
                
                i++;
            }

            return indices;
        }

        //-----------------------$SwapChain----------------------//
        //-------------------------------------------------------//
        //-------------------------------------------------------//

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
        {
            SwapChainSupportDetails details;

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilites);

            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

            if(formatCount != 0)
            {
                details.formats.resize(formatCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
            }

            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

            if(presentModeCount != 0)
            {
                details.presentModes.resize(presentModeCount);
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
            }

            return details;
        }

        void createSwapChain()
        {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);


            uint32_t imageCount = swapChainSupport.capabilites.minImageCount + 1;

            if(swapChainSupport.capabilites.maxImageCount > 0 
            && imageCount > swapChainSupport.capabilites.maxImageCount) 
            {
                imageCount = swapChainSupport.capabilites.maxImageCount;
            }
            
            QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
            uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                indices.presentFamily.value()};

            VkFormat format;
            VkExtent2D extent;

            VkSwapchainCreateInfoKHR createInfo{};
            populateSwapchainCreateInfo(createInfo, 
                swapChainSupport.formats, swapChainSupport.presentModes, swapChainSupport.capabilites,
                width, height, imageCount, surface,
                queueFamilyIndices, format, extent);

            if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create swapchain");
            }

            swapChainImageFormat = format;
            swapChainExtent = extent;

            vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
            swapChainImages.resize(imageCount);
            vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
        } 

        void recreateSwapChain()
        {
            int width = 0, height = 0;
            glfwGetFramebufferSize(window, &width, &height);
            while(width == 0 || height == 0)
            {
                glfwGetFramebufferSize(window, &width, &height);
                glfwWaitEvents();
            }

            vkDeviceWaitIdle(device);

            cleanupSwapChain();
            createSwapChain();
            createImageViews();
            createFramebuffers();
        }

        void cleanupSwapChain()
        {
            for(size_t i = 0; i < swapChainFramebuffers.size(); i++)
            {
                vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
            }

            for (size_t i = 0; i < swapChainImageViews.size(); i++)
            {
                vkDestroyImageView(device, swapChainImageViews[i], nullptr);
            }

            vkDestroySwapchainKHR(device, swapChain, nullptr);
        }

        //-----------------------$ImageViews----------------------//
        //--------------------------------------------------------//
        //--------------------------------------------------------//

        void createImageViews()
        {
            swapChainImageViews.resize(swapChainImages.size());

            for(size_t i = 0; i < swapChainImages.size(); i++)
            {
                VkImageViewCreateInfo createInfo{};
                populateImageViewCreateInfo(createInfo, swapChainImages[i], swapChainImageFormat);

                if(vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
                { throw std::runtime_error("failed to create image view"); }
            }
        }

        //-----------------------$Pipeline----------------------//
        //------------------------------------------------------//
        //------------------------------------------------------//

        void createRenderPass()
        {
            VkAttachmentDescription colorAttachment{};
            populateColorAttachment(colorAttachment, swapChainImageFormat);

            VkAttachmentReference colorAttachmentRef{};
            populateColorAttachmentRef(colorAttachmentRef);

            VkSubpassDescription subpass{};
            populateSubpass(subpass, colorAttachmentRef);

            VkSubpassDependency dependency{};
            populateSubpassDependency(dependency);

            VkRenderPassCreateInfo renderPassInfo{};
            populateRenderPassCreateInfo(renderPassInfo, colorAttachment, subpass, dependency);

            if(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
            { throw std::runtime_error("failed to create render pass"); }
        }

        void createGraphicsPipeline()
        {
            std::vector<char> vertShaderCode = readFile("shaders/vert.spv");
            std::vector<char> fragShaderCode = readFile("shaders/frag.spv");

            VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
            VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            populateVertShaderStageCreateInfo(vertShaderStageInfo, vertShaderModule);

            VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
            populateFragShaderStageCreateInfo(fragShaderStageInfo, fragShaderModule);

            VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

            std::vector<VkDynamicState> dynamicStates = 
            {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };

            VkPipelineDynamicStateCreateInfo dynamicState{};
            populatePipelineDynamicStateCreateInfo(dynamicState, dynamicStates);

            auto bindingDescription = Vertex::getBindingDescription();
            auto attributeDescriptions = Vertex::getAttributeDescriptions(); 
            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            populatePipelineVertexInputStateCreateInfo(vertexInputInfo,
                bindingDescription, attributeDescriptions);

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            populatePipelineInputAssemblyStateCreateInfo(inputAssembly);

            VkViewport viewport{};
            populateViewport(viewport, swapChainExtent);

            VkRect2D scissor{};
            populateScissor(scissor, swapChainExtent);

            VkPipelineViewportStateCreateInfo viewportState{};
            populatePipelineViewportStateCreateInfo(viewportState, viewport, scissor);

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            populatePipelineRasterizationStateCreateInfo(rasterizer);

            VkPipelineMultisampleStateCreateInfo multisampling{};
            populatePipelineMultisampleStateCreateInfo(multisampling);

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            populatePipelineColorBlendAttachmentState(colorBlendAttachment);

            VkPipelineColorBlendStateCreateInfo colorBlending{};
            populatePipelineColorBlendStateCreateInfo(colorBlending, colorBlendAttachment);

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            populatePipelineLayoutCreateInfo(pipelineLayoutInfo, descriptorSetLayout);

            if(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
            { throw std::runtime_error("failed to create pipeline layout"); }

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = nullptr;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &dynamicState;
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
            pipelineInfo.basePipelineIndex = -1;

            if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
            { throw std::runtime_error("failed to create graphics pipeline"); }

            vkDestroyShaderModule(device, vertShaderModule, nullptr);
            vkDestroyShaderModule(device, fragShaderModule, nullptr);
        }

        VkShaderModule createShaderModule(const std::vector<char>& code)
        {
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

            VkShaderModule shaderModule;
            if(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            { throw std::runtime_error("failed to create shader module"); }

            return shaderModule;
        }

        //-----------------------$Framebuffers----------------------//
        //----------------------------------------------------------//
        //----------------------------------------------------------//

        void createFramebuffers()
        {
            swapChainFramebuffers.resize(swapChainImageViews.size());

            for(size_t i = 0; i < swapChainImageViews.size(); i++)
            {
                VkImageView attachments[] = 
                {
                    swapChainImageViews[i]
                };

                VkFramebufferCreateInfo framebufferInfo{};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = renderPass;
                framebufferInfo.attachmentCount = 1;
                framebufferInfo.pAttachments = attachments;
                framebufferInfo.width = swapChainExtent.width;
                framebufferInfo.height = swapChainExtent.height;
                framebufferInfo.layers = 1;

                if(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
                { throw std::runtime_error("failed to create framebuffer"); }
            }
        }

        //-----------------------$CommandPool----------------------//
        //---------------------------------------------------------//
        //---------------------------------------------------------//

        void createCommandPool()
        {
            QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

            if(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
            { throw std::runtime_error("failed to create command pool"); }
        }

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0;
            beginInfo.pInheritanceInfo = nullptr;

            if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
            { throw std::runtime_error("failed to begin recording command buffer"); }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChainExtent;
            VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(swapChainExtent.width);
            viewport.height = static_cast<float>(swapChainExtent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = swapChainExtent;
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            VkBuffer vertexBuffers[] = {vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);

            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

            vkCmdEndRenderPass(commandBuffer);

            if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
            { throw std::runtime_error("failed to record command buffer"); }
        }

        void createCommandBuffers()
        {
            commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = commandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

            if(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
            { throw std::runtime_error("failed to allocate command buffers"); }
        }

        void createSyncObjects()
        {
            imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
                if(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS
                || vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS
                || vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
                { throw std::runtime_error("error creating semaphores"); }
            }
        }

        //-----------------------$Buffers----------------------//
        //-----------------------------------------------------//
        //-----------------------------------------------------//
        
        void createVertexBuffer()
        {
            VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
            
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(device, physicalDevice, bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

            void* data;
            vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, vertices.data(), (size_t) bufferSize);
            vkUnmapMemory(device, stagingBufferMemory);

            createBuffer(device, physicalDevice, bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer, vertexBufferMemory);

            copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
        }

        void createIndexBuffer()
        {
            VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
            
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(device, physicalDevice, bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

            void* data;
            vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, indices.data(), (size_t) bufferSize);
            vkUnmapMemory(device, stagingBufferMemory);

            createBuffer(device, physicalDevice, bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer, indexBufferMemory);

            copyBuffer(stagingBuffer, indexBuffer, bufferSize);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
        }

        void createUniformBuffers()
        {
            VkDeviceSize bufferSize = sizeof(UniformBufferObject);

            uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
            uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
            uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

            for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                createBuffer(device, physicalDevice, bufferSize,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    uniformBuffers[i], uniformBuffersMemory[i]);

                vkMapMemory(device, uniformBuffersMemory[i], 0 , bufferSize, 0, 
                    &uniformBuffersMapped[i]);
            }
        }

        void updateUniformBuffer(uint32_t currentImage)
        {
            static auto startTime = std::chrono::high_resolution_clock::now();

            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>
                (currentTime - startTime).count();

            UniformBufferObject ubo{};
            ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            
            ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f));

            ubo.proj = glm::perspective(glm::radians(45.0f), 
                swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10.0f);

            ubo.proj[1][1] *= -1;

            memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
        }

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
        {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = commandPool;
            allocInfo.commandBufferCount = 1;

            VkCommandBuffer commandBuffer;
            vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = size;
            vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(graphicsQueue);

            vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
        }

        void drawFrame()
        {
            vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

            uint32_t imageIndex;
            VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, 
                imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

            if(result == VK_ERROR_OUT_OF_DATE_KHR)
            {
                recreateSwapChain();
                return;
            }
            else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            {
                throw std::runtime_error("failed to acquire swap chain image");
            }

            updateUniformBuffer(currentFrame);
            
            vkResetFences(device, 1, &inFlightFences[currentFrame]);

            vkResetCommandBuffer(commandBuffers[currentFrame], 0);

            recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
            VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
            { throw std::runtime_error("failed to submit draw command"); }

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;
            VkSwapchainKHR swapChains[] = {swapChain};
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr;

            result = vkQueuePresentKHR(presentQueue, &presentInfo);

            if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR 
                || frambufferResized)
            {
                frambufferResized = false;
                recreateSwapChain();
            }
            else if(result != VK_SUCCESS)
            {
                throw std::runtime_error("failed to present swap chain image");
            }

            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        }

        void createSurface()
        {
            if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create surface");
            }
        }

        void createDescriptorSetLayout()
        {
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            populateUniformBufferObjectLayoutBinding(uboLayoutBinding);

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            populateDescriptorSetLayoutCreateInfo(layoutInfo, uboLayoutBinding);

            if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
            { throw std::runtime_error("failed to create descriptor set layout"); }
        }

        void createDescriptorPool()
        {
            VkDescriptorPoolSize poolSize{};
            poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = 1;
            poolInfo.pPoolSizes = &poolSize;
            poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

            if(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
            { throw std::runtime_error("failed to create descriptor pool"); }
        }

        void createDescriptorSets()
        {
            std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
            
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.descriptorPool = descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
            allocInfo.pSetLayouts = layouts.data();

            descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
            if(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
            { throw std::runtime_error("failed to allocate descriptor sets"); }

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = uniformBuffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(UniformBufferObject);

                VkWriteDescriptorSet descriptorWrite{};
                populateWriteDescriptorSet(descriptorWrite, descriptorSets[i], bufferInfo);

                vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
            }
            
        }
        
        void initVulkan()
        {
            createInstance();
            if(enableValidationLayers)
            { setupDebugMessenger(instance, debugMessenger); }
            createSurface();
            pickPhysicalDevice();
            createLogicalDevice();
            createSwapChain();
            createImageViews();
            createRenderPass();
            createDescriptorSetLayout();
            createGraphicsPipeline();
            createFramebuffers();
            createCommandPool();
            createVertexBuffer();
            createIndexBuffer();
            createUniformBuffers();
            createDescriptorPool();
            createDescriptorSets();
            createCommandBuffers();
            createSyncObjects();
        }

        void mainLoop()
        {
            while(!glfwWindowShouldClose(window))
            {
                glfwPollEvents();
                drawFrame();
            }

            vkDeviceWaitIdle(device);
        }

        void cleanup()
        {
            cleanupSwapChain();

            for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                vkDestroyBuffer(device, uniformBuffers[i], nullptr);
                vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
            }

            vkDestroyDescriptorPool(device, descriptorPool, nullptr);

            vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

            vkDestroyBuffer(device, vertexBuffer, nullptr);
            vkFreeMemory(device, vertexBufferMemory, nullptr);

            vkDestroyBuffer(device, indexBuffer, nullptr);
            vkFreeMemory(device, indexBufferMemory, nullptr);

            for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
                vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
                vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
                vkDestroyFence(device, inFlightFences[i], nullptr);
            }

            vkDestroyCommandPool(device, commandPool, nullptr);

            vkDestroyPipeline(device, graphicsPipeline, nullptr);
            vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
            vkDestroyRenderPass(device, renderPass, nullptr);

            vkDestroyDevice(device, nullptr);

            if(enableValidationLayers)
            {
                DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
            }

            vkDestroySurfaceKHR(instance, surface, nullptr);

            vkDestroyInstance(instance, nullptr);

            glfwDestroyWindow(window);

            glfwTerminate();
        }
};