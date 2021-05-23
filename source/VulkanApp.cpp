#include <iostream>
#include <sstream>
#include "VulkanApp.h"

VulkanApp::~VulkanApp()
{
    
    vkDestroySwapchainKHR(device, screenBufferResources.swapChain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDeviceWaitIdle(device);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}

void VulkanApp::Init()
{
    glfwInit();
    createInstance();
    createPhysicalDevice();
    createWindow();
    getQueueFamily();
    createDevice();
    createSwapchain();

    createRenderPass();


}

void VulkanApp::createInstance()
{
    std::vector<const char*> instanceExtensions;
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    instanceExtensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
    
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Animation";
    appInfo.applicationVersion = 0;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount   = uint32_t(instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if(result != VK_SUCCESS) 
        RUN_TIME_ERROR("No instance created\n");

}

void VulkanApp::createPhysicalDevice()
{
    uint32_t physicalDeviceCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if (result != VK_SUCCESS)
        RUN_TIME_ERROR("Error enumerating physical devices\n");
    std::cerr << "Number of physical devices is " << physicalDeviceCount << std::endl;
    if (physicalDeviceCount == 0)
        RUN_TIME_ERROR("There no physical devices: physicalDeviceCount = 0\n");
    std::vector<VkPhysicalDevice> devices(physicalDeviceCount);
    result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, devices.data());
    if (result != VK_SUCCESS)
        RUN_TIME_ERROR("Error enumerating physical devices\n");

    physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;

    for (uint32_t i = 0; i < devices.size(); i++) {
        vkGetPhysicalDeviceProperties(devices[i], &properties);
        vkGetPhysicalDeviceFeatures(devices[i], &features);
        std::cerr << "device " << i << ", name = " << properties.deviceName << std::endl;
    }
    physicalDevice = devices[0];

    if (physicalDevice == VK_NULL_HANDLE)
        RUN_TIME_ERROR("Error selecting physical devices\n");

}

void VulkanApp::getQueueFamily()
{
    uint32_t queueFamCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamCount, nullptr); 
    if (queueFamCount == 0)
        RUN_TIME_ERROR("No queue families on this physical device\n");
    std::vector<VkQueueFamilyProperties> queueFamPoperties(queueFamCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamCount, queueFamPoperties.data());

    queueFamilyIdx = -1;
    std::cerr << "Available queue families:" << std::endl;
    for (uint32_t i = 0; i < queueFamPoperties.size(); i++) {
        std::cerr << std::boolalpha << "\tFamily number " << i << ":\n\t\tsupports " << 
            queueFamPoperties[i].queueCount << " queue(s)" << std::endl <<
            "\t\tgraphics support = " << ((bool)(queueFamPoperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) << std::endl <<
            "\t\tcompute support = " << ((bool)(queueFamPoperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)) << std::endl <<
            "\t\ttransfer support = " << ((bool)(queueFamPoperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)) << std::endl <<
            "\t\tsparce binding support = " << ((bool)(queueFamPoperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)) << std::endl; 
            
            if ((queueFamPoperties[i].queueFlags & requiredQuequeProps) && queueFamilyIdx == -1)
                queueFamilyIdx = i;
    } if (queueFamilyIdx == -1)
        RUN_TIME_ERROR("There is no families supporting requirements\n");

    //// check if chosen famili idx support surface 
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIdx, surface, &presentSupport);
    if (!presentSupport)
      throw std::runtime_error("vkGetPhysicalDeviceSurfaceSupportKHR: no present support for the target device and graphics queue");

}

void VulkanApp::createDevice()
{
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = nullptr;
    queueCreateInfo.queueFamilyIndex = queueFamilyIdx;
    queueCreateInfo.queueCount = 1;
    float queuePriorities = 1.0;
    queueCreateInfo.pQueuePriorities = &queuePriorities;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.pQueueCreateInfos = &queueCreateInfo;  
    createInfo.queueCreateInfoCount = 1;
    createInfo.enabledExtensionCount   = uint32_t(DEVICE_EXTENTIONS.size());
    createInfo.ppEnabledExtensionNames = DEVICE_EXTENTIONS.data();

    VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
    if (result != VK_SUCCESS)
        RUN_TIME_ERROR("Error creating logical device\n");

}

void VulkanApp::createWindow()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    if (glfwVulkanSupported() != GLFW_TRUE) 
        RUN_TIME_ERROR("Error glfw do not support vulkan");


    /*VkBool32 presentSupport = false;
    uint32_t i = 0;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
    std::cerr << presentSupport << i;*/

    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        RUN_TIME_ERROR("Error glfwCreateWindowSurface failed to create window surface!");
}

void VulkanApp::createSwapchain()
{

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    uint32_t formatCount;
    uint32_t presentModeCount;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
    }
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
    }

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(formats);
    VkExtent2D extent = chooseSwapExtent(surfaceCapabilities, WIDTH, HEIGHT);
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.surface = surface;
    createInfo.minImageCount = chooseImageCount(surfaceCapabilities);
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.preTransform = surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = chooseSwapPresentMode(presentModes);
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &screenBufferResources.swapChain) != VK_SUCCESS)
        RUN_TIME_ERROR("Error failed to create swap chain!");

    screenBufferResources.swapChainImageFormat = surfaceFormat.format;
    screenBufferResources.swapChainExtent = extent;

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(device, screenBufferResources.swapChain, &imageCount, nullptr);
    screenBufferResources.swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, screenBufferResources.swapChain, &imageCount, screenBufferResources.swapChainImages.data());

}

void VulkanApp::createRenderPass()
  {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format         = screenBufferResources.swapChainImageFormat;
    colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass  = {};
    subpass.pipelineBindPoint     = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount  = 1;
    subpass.pColorAttachments     = &colorAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments    = &colorAttachment;
    renderPassInfo.subpassCount    = 1;
    renderPassInfo.pSubpasses      = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies   = &dependency;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        RUN_TIME_ERROR("CreateRenderPass: failed to create render pass!");
  }

VkDevice& VulkanApp::operator()()
{
    return device;
}


static void RunTimeError(const char* file, int line, const char* msg)
{
    std::stringstream strout;
    strout << "runtime_error at " << file << ", line " << line << ": " << msg << std::endl;
    throw std::runtime_error(strout.str().c_str());
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && 
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
{
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        } else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height) 
{
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = { uint32_t(width), uint32_t(height) };

        actualExtent.width  = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

uint32_t chooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities)
{
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }
    return imageCount;
}


