#ifndef VULKAN_APP_H
#define VULKAN_APP_H

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#ifdef WIN32
#pragma comment(lib,"glfw3.lib")
#endif

#include <vulkan/vulkan.h>
#include <vector>

const int WIDTH  = 600;
const int HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> DEVICE_EXTENTIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct ScreenBufferResources
{
    VkSwapchainKHR             swapChain;
    std::vector<VkImage>       swapChainImages;
    VkFormat                   swapChainImageFormat;
    VkExtent2D                 swapChainExtent;
    std::vector<VkImageView>   swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
};

  struct SyncObj
  {
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence>     inFlightFences;
  };

class VulkanApp
{
public:
    explicit VulkanApp() : 
        requiredQuequeProps(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT){};
    ~VulkanApp();
    void Init();
    VkDevice& operator()();


private:
    const VkQueueFlags    requiredQuequeProps;
    VkInstance            instance;
    VkPhysicalDevice      physicalDevice;
    VkDevice              device;
    uint32_t              queueFamilyIdx;
    VkQueue               graphicsQueue;
    VkQueue               presentQueue;

    GLFWwindow*           window;
    VkSurfaceKHR          surface;
    ScreenBufferResources screenBufferResources;

    VkBuffer              vertexBuffer;
    VkDeviceMemory        vertexMemory;

    SyncObj               syncObj;

    VkRenderPass          renderPass;
    VkPipelineLayout      pipelineLayout;
    VkPipeline            pipeline;

    void createInstance();
    void createPhysicalDevice();
    void getQueueFamily();
    void createDevice();
    void checkProperties();
    void createWindow();
    void createSwapchain();
    void createScreenImageViews();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFrameBuffer();
    void createVertexBuffer();
    void createSyncObjects();


    VkShaderModule createShaderModule(const std::vector<uint32_t>& code);

};

static void RunTimeError(const char* file, int line, const char* msg);

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);
uint32_t chooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities);
void loadShaderModule(const char* filename, std::vector<uint32_t>& data);

#undef  RUN_TIME_ERROR
#undef  RUN_TIME_ERROR_AT
#define RUN_TIME_ERROR(e) (RunTimeError(__FILE__,__LINE__,(e)))
#define RUN_TIME_ERROR_AT(e, file, line) (RunTimeError((file),(line),(e)))

#endif //VULKAN_APP_H

