#include <iostream>
#include <vector>
#include <sstream>
#include "VulkanApp.h"


VulkanApp::~VulkanApp()
{
    vkDeviceWaitIdle(device);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void VulkanApp::Init()
{
    createInstance();
    createPhysicalDevice();
    getQueueFamily();
    createDevice();
}

void VulkanApp::createInstance()
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

    VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
    if (result != VK_SUCCESS)
        RUN_TIME_ERROR("Error creating logical device\n");

}

static void RunTimeError(const char* file, int line, const char* msg)
{
    std::stringstream strout;
    strout << "runtime_error at " << file << ", line " << line << ": " << msg << std::endl;
    throw std::runtime_error(strout.str().c_str());
}