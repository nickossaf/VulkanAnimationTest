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
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <array>

const int WIDTH  = 800;
const int HEIGHT = 800;
const int MAX_FRAMES_IN_FLIGHT = 3;

static char g_validationLayerData[256];
static const char* g_debugReportExtName  = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;

const std::vector<const char*> DEVICE_EXTENTIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct Image
{
    int w;
    int h;
    int c;
    unsigned char* image;
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
    void Run();
    VkDevice& operator()();

private:
    const VkQueueFlags           requiredQuequeProps;
    VkInstance                   instance;
    VkPhysicalDevice             physicalDevice;
    VkDevice                     device;
    uint32_t                     queueFamilyIdx;
    VkQueue                      graphicsQueue;
    VkQueue                      presentQueue;

    GLFWwindow*                  window;
    VkSurfaceKHR                 surface;
    ScreenBufferResources        screenBufferResources;
    size_t                       currentFrame;

    /*std::vector<float>           vertices = {
        -0.5f, -0.5f,0,0,0,0,0,
        0.5f, -0.5f,0,0,0,0,0,
        0.0f, +0.5f,0,0,0,0,0,
    };
    std::vector<uint16_t>        vertIdxs = {
        0, 1, 2
    };*/
    
    std::vector<float>           vertices;
    std::vector<uint16_t>        vertIdxs;

    VkBuffer                     vertexBuffer;
    VkDeviceMemory               vertexMemory;
    VkBuffer                     idxBuffer;
    VkDeviceMemory               idxMemory;

    Image                        grassTextureImage;
    VkImage                      textureImage;
    VkDeviceMemory               textureImageMemory;
    VkImageView                  textureImageView;
    VkSampler                    textureSampler;
    
    VkBuffer                     stagingBuffer;
    VkDeviceMemory               stagingBufferMemory;

    SyncObj                      syncObj;
    VkCommandPool                commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    VkDescriptorPool             descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    VkRenderPass                 renderPass;
    VkDescriptorSetLayout        descriptorSetLayout;
    VkPipelineLayout             pipelineLayout;
    VkPipeline                   pipeline;

    void initResources();
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
    void createIndexBuffer();
    void createSyncObjects();
    void createCommandPool();
    void createCommandBuffers();
    void createDescriptorSetLayout();
    void createDescriptorSets();
    void createDescriptorPool();
    void copyVertices2GPU();
    void createTexture();
    void createStagingBuffer();
    void drawFrame();

    VkShaderModule createShaderModule(const std::vector<uint32_t>& code);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(
    VkDebugReportFlagsEXT                       flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char*                                 pLayerPrefix,
    const char*                                 pMessage,
    void*                                       pUserData)
    {
        printf("[Debug Report]: %s: %s\n", pLayerPrefix, pMessage);
        return VK_FALSE;
    };
    VkDebugReportCallbackEXT debugReportCallback;
    std::vector<const char*> enabledLayers;
    void initDebugReportCallback();

};

static void RunTimeError(const char* file, int line, const char* msg);

void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandBuffer commandBuffer);
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

