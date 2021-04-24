#ifndef VULKAN_APP_H
#define VULKAN_APP_H

#pragma once

#include <vulkan/vulkan.h>

class VulkanApp
{
public:
    explicit VulkanApp() : 
        requiredQuequeProps(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT){};
    ~VulkanApp();
    void Init();

private:
    const VkQueueFlags requiredQuequeProps;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    uint32_t queueFamilyIdx;

    void createInstance();
    void createPhysicalDevice();
    void getQueueFamily();
    void createDevice();
};

static void RunTimeError(const char* file, int line, const char* msg);

#undef  RUN_TIME_ERROR
#undef  RUN_TIME_ERROR_AT
#define RUN_TIME_ERROR(e) (RunTimeError(__FILE__,__LINE__,(e)))
#define RUN_TIME_ERROR_AT(e, file, line) (RunTimeError((file),(line),(e)))

#endif //VULKAN_APP_H

