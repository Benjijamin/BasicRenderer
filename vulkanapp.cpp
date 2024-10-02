#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <string.h>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include <utils.h>
#include <vkstructs.h>
#include <vkdebug.h>

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
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkCommandPool commandPool;

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

            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            populatePipelineVertexInputStateCreateInfo(vertexInputInfo);

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
            populatePipelineLayoutCreateInfo(pipelineLayoutInfo);

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

            vkCmdDraw(commandBuffer, 3, 1, 0, 0);

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
            createGraphicsPipeline();
            createFramebuffers();
            createCommandPool();
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