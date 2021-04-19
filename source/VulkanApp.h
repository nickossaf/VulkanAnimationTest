#include <vulkan/vulkan.h>

class VulkanApp
{
public:
    explicit VulkanApp(){};
    ~VulkanApp();
    VkResult Init();

private:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;

    VkResult createInstance();
    VkResult createPhysicalDevice();
    VkResult createDevice();
};
