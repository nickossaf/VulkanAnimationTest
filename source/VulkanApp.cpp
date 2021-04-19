#include <iostream>
#include "VulkanApp.h"


VulkanApp::~VulkanApp()
{
    vkDestroyInstance(instance, nullptr);
}

VkResult VulkanApp::Init()
{
    createInstance();
    return createPhysicalDevice();
}

VkResult VulkanApp::createInstance()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "RayTracing";
    appInfo.applicationVersion = 0;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (result != VK_SUCCESS) {
        //TODO: handle error
    }

    return result;

}

VkResult VulkanApp::createPhysicalDevice()
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    std::cout << "number of devices is " << physicalDeviceCount << std::endl;
    return VK_SUCCESS;    
}

VkResult VulkanApp::createDevice()
{
    return VK_SUCCESS;
}