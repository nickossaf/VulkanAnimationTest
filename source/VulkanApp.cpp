#include "VulkanApp.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

VulkanApp::~VulkanApp()
{
    vkFreeMemory(device, vertexMemory, NULL);
    vkDestroyBuffer(device, vertexBuffer, NULL);
    vkFreeMemory(device, idxMemory, NULL);
    vkDestroyBuffer(device, idxBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, textureImageMemory, NULL);
    vkDestroySampler(device, textureSampler, nullptr);
    vkDestroyImage(device, textureImage, NULL);
    vkDestroyImageView(device, textureImageView, nullptr);
    for (size_t i = 0; i < screenBufferResources.swapChainImages.size(); i++) {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);


    auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func == nullptr)
        RUN_TIME_ERROR("Could not load vkDestroyDebugReportCallbackEXT");
    func(instance, debugReportCallback, NULL);

    vkDestroyCommandPool(device, commandPool, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, syncObj.renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, syncObj.imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, syncObj.inFlightFences[i], nullptr);
    }
    
    for (auto framebuffer : screenBufferResources.swapChainFramebuffers) {
      vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    for (auto imageView : screenBufferResources.swapChainImageViews) {
      vkDestroyImageView(device, imageView, nullptr);
    }
    
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    
    vkDestroySwapchainKHR(device, screenBufferResources.swapChain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);

    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    
    glfwDestroyWindow(window);
    glfwTerminate();
}

void VulkanApp::Init()
{
    glfwInit();
    initResources();
    createInstance();
    initDebugReportCallback();
    createPhysicalDevice();
    createWindow();
    getQueueFamily();
    createDevice();
    createSwapchain();

    createRenderPass();
    createDescriptorSetLayout(); //#
    createGraphicsPipeline();

    createFrameBuffer();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createSyncObjects();
    createTexture();

    createDescriptorPool(); //#
    createDescriptorSets(); //#

    createCommandPool();
    createCommandBuffers();

    copyVertices2GPU();
}

void VulkanApp::Run()
{
    currentFrame = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(device);
}

void VulkanApp::initResources()
{

    std::ifstream vertFile("../resource/vertex2.txt");
    if (!vertFile.is_open())
        RUN_TIME_ERROR("error loading configured vertices");

    while (!vertFile.eof()) {
        float vert;
        vertFile >> vert;
        vertices.push_back(vert); 
    }

    vertFile.close();

    std::ifstream idxFile("../resource/index.txt");
    if (!idxFile.is_open())
        RUN_TIME_ERROR("error loading configured vert indexes");

    while (!idxFile.eof()) {
        uint16_t idx;
        idxFile >> idx;
        vertIdxs.push_back(idx); 
    }
    idxFile.close();

}

void VulkanApp::createInstance()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    std::vector<VkLayerProperties> layerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

    bool foundLayer = false;
    for (VkLayerProperties prop : layerProperties) {
        std::cout << prop.layerName <<std::endl;
        if (strcmp("VK_LAYER_LUNARG_standard_validation", prop.layerName) == 0 || 
            strcmp("VK_LAYER_KHRONOS_validation", prop.layerName) == 0) {
            strncpy(g_validationLayerData, prop.layerName, 256);
            foundLayer = true;
            break;
        }

    }

    if (!foundLayer)
      RUN_TIME_ERROR("Layer VK_LAYER_LUNARG_standard_validation not supported\n");

    enabledLayers.push_back(g_validationLayerData); 
    uint32_t extensionCount;

    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    std::vector<VkExtensionProperties> extensionProperties(extensionCount);
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensionProperties.data());

    bool foundExtension = false;
    for (VkExtensionProperties prop : extensionProperties) {
        if (strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, prop.extensionName) == 0) {
            foundExtension = true;
            break;
        }

    }

    if (!foundExtension)
      RUN_TIME_ERROR("Extension VK_EXT_DEBUG_REPORT_EXTENSION_NAME not supported\n");

    
    std::vector<const char*> instanceExtensions;
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    instanceExtensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
    instanceExtensions.push_back(g_debugReportExtName);

    
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
    createInfo.enabledLayerCount = uint32_t(enabledLayers.size());
    createInfo.ppEnabledLayerNames = enabledLayers.data();

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

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.pQueueCreateInfos = &queueCreateInfo;  
    createInfo.queueCreateInfoCount = 1;
    createInfo.enabledExtensionCount   = uint32_t(DEVICE_EXTENTIONS.size());
    createInfo.ppEnabledExtensionNames = DEVICE_EXTENTIONS.data();
    createInfo.enabledLayerCount = uint32_t(enabledLayers.size());
    createInfo.ppEnabledLayerNames = enabledLayers.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
    if (result != VK_SUCCESS)
        RUN_TIME_ERROR("Error creating logical device\n");

    vkGetDeviceQueue(device, queueFamilyIdx, 0, &graphicsQueue);
    vkGetDeviceQueue(device, queueFamilyIdx, 0, &presentQueue);

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

    createScreenImageViews();
}

void VulkanApp::createScreenImageViews()
{
    screenBufferResources.swapChainImageViews.resize(screenBufferResources.swapChainImages.size());
    for (size_t i = 0; i < screenBufferResources.swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = screenBufferResources.swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = screenBufferResources.swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &createInfo, nullptr, &screenBufferResources.swapChainImageViews[i]) != VK_SUCCESS)
            RUN_TIME_ERROR("createScreenImageViews failed to create image views!");
    }

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

void VulkanApp::createGraphicsPipeline()
{
    ////load shader modules
    std::vector<uint32_t> vertShaderCode;
    std::vector<uint32_t> fragShaderCode;
    loadShaderModule("../shaders/vert.spv", vertShaderCode);
    loadShaderModule("../shaders/frag.spv", fragShaderCode);
    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);   

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkVertexInputBindingDescription inputBinding = { };
    inputBinding.binding = 0;
    inputBinding.stride = sizeof(float) * 4;
    inputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributes[] = {
        {0, 0, VK_FORMAT_R32G32_SFLOAT, 0},
        {1, 0, VK_FORMAT_R32G32_SFLOAT, 2*sizeof(float)}
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexBindingDescriptions = &inputBinding;
    vertexInputInfo.pVertexAttributeDescriptions = attributes;
    
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    //Viewport
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)WIDTH;
    viewport.height = (float)HEIGHT;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = screenBufferResources.swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    //Rasterisation
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE; // VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    //Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    //ColorBlend
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;
    

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //pipelineLayoutInfo.setLayoutCount = 0;
    //pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;


    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        RUN_TIME_ERROR("createGraphicsPipeline: failed to create pipeline layout!");

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
        RUN_TIME_ERROR("createGraphicsPipeline: failed to create graphics pipeline!");

    vkDestroyShaderModule(device, vertShaderModule, nullptr);
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
}

void VulkanApp::createFrameBuffer()
{
    screenBufferResources.swapChainFramebuffers.resize(screenBufferResources.swapChainImageViews.size());

    for (size_t i = 0; i < screenBufferResources.swapChainImageViews.size(); i++) {
        VkImageView attachments[] = { screenBufferResources.swapChainImageViews[i] };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = screenBufferResources.swapChainExtent.width;
        framebufferInfo.height = screenBufferResources.swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &screenBufferResources.swapChainFramebuffers[i]) != VK_SUCCESS)
            RUN_TIME_ERROR("failed to create framebuffer!");
    }
}

void VulkanApp::createVertexBuffer()
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.size = vertices.size() * sizeof(float);                         
    bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;            

    if (vkCreateBuffer(device, &bufferCreateInfo, NULL, &vertexBuffer) != VK_SUCCESS)
        RUN_TIME_ERROR("Error creating vertex buffer");
    
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memoryRequirements);

    uint32_t memoryTypeIndex = -1;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
        if ((memoryRequirements.memoryTypeBits & (1 << i)) &&
            ((memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
                memoryTypeIndex = i;
                break;
            }
    }

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(device, &allocateInfo, nullptr, &vertexMemory) != VK_SUCCESS)
        RUN_TIME_ERROR("Error allocating vertex memmory");
  

    if (vkBindBufferMemory(device, vertexBuffer, vertexMemory, 0) != VK_SUCCESS)
        RUN_TIME_ERROR("Error binding vertex memmory");
  
}

void VulkanApp::createIndexBuffer()
{
    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.size = vertIdxs.size() * sizeof(uint16_t);                         
    bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;            

    if (vkCreateBuffer(device, &bufferCreateInfo, NULL, &idxBuffer) != VK_SUCCESS)
        RUN_TIME_ERROR("Error creating index buffer");
    
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, idxBuffer, &memoryRequirements);

    uint32_t memoryTypeIndex = -1;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
        if ((memoryRequirements.memoryTypeBits & (1 << i)) &&
            ((memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
                memoryTypeIndex = i;
                break;
            }
    }

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(device, &allocateInfo, nullptr, &idxMemory) != VK_SUCCESS)
        RUN_TIME_ERROR("Error allocating idx memmory");
  

    if (vkBindBufferMemory(device, idxBuffer, idxMemory, 0) != VK_SUCCESS)
        RUN_TIME_ERROR("Error binding idx memmory");
  
}

void VulkanApp::createUniformBuffers() 
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(screenBufferResources.swapChainImages.size());
    uniformBuffersMemory.resize(screenBufferResources.swapChainImages.size());

    for (size_t i = 0; i < screenBufferResources.swapChainImages.size(); i++) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &uniformBuffers[i]) != VK_SUCCESS) 
            RUN_TIME_ERROR("failed to create uniform buffer!");
    
        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, uniformBuffers[i], &memoryRequirements);

        uint32_t prop = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        uint32_t memoryTypeIndex = -1;
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
        {
            if ((memoryRequirements.memoryTypeBits & (1 << i)) &&
                ((memoryProperties.memoryTypes[i].propertyFlags & prop) == prop)) {
                    memoryTypeIndex = i;
                    break;
                }
        }

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;

        if (vkAllocateMemory(device, &allocInfo, nullptr, &uniformBuffersMemory[i]) != VK_SUCCESS) 
            RUN_TIME_ERROR("failed to allocate uniform buffer memory!");

        vkBindBufferMemory(device, uniformBuffers[i], uniformBuffersMemory[i], 0);
    }
}

void VulkanApp::createSyncObjects()
{
    syncObj.imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    syncObj.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    syncObj.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &syncObj.imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &syncObj.renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence    (device, &fenceInfo,     nullptr, &syncObj.inFlightFences[i]) != VK_SUCCESS) {
            RUN_TIME_ERROR("createSyncObjects failed to create synchronization objects for a frame!");
        }
    }
}

void VulkanApp::createCommandPool()
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIdx;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        RUN_TIME_ERROR("createCommandPool failed to create command pool!");    
}

void VulkanApp::createCommandBuffers() 
{
    commandBuffers.resize(screenBufferResources.swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        RUN_TIME_ERROR("createCommandBuffers: failed to allocate command buffers!");

    for (size_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) 
            RUN_TIME_ERROR("createCommandBuffers: failed to begin recording command buffer!");

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = screenBufferResources.swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = screenBufferResources.swapChainExtent;

        VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        VkBuffer vertexBuffers[] = { vertexBuffer };
        VkDeviceSize offsets[]   = { 0 };
        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffers[i], idxBuffer, 0, VK_INDEX_TYPE_UINT16);
        vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
        //vkCmdDraw(commandBuffers[i], vertices.size(), 1, 0, 0);
        vkCmdDrawIndexed(commandBuffers[i], vertIdxs.size(), 2, 0, 0, 0);
        vkCmdEndRenderPass(commandBuffers[i]);

        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) 
            RUN_TIME_ERROR("failed to record command buffer!");
        
    }
}

void VulkanApp::createTexture()
{
    grassTextureImage.image = stbi_load("../resource/grass-texture.png", &grassTextureImage.w, 
                                                                         &grassTextureImage.h, 
                                                                         &grassTextureImage.c, 
                                                                         STBI_rgb_alpha);
    VkDeviceSize imageSize = grassTextureImage.w * grassTextureImage.h * 4;
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(grassTextureImage.w);
    imageInfo.extent.height = static_cast<uint32_t>(grassTextureImage.h);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    if (vkCreateImage(device, &imageInfo, nullptr, &textureImage) != VK_SUCCESS) 
        RUN_TIME_ERROR("failed to create texture image!");

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device, textureImage, &memoryRequirements);

    uint32_t memoryTypeIndex = -1;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
        if ((memoryRequirements.memoryTypeBits & (1 << i)) &&
            ((memoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
                memoryTypeIndex = i;
                break;
            }
    }

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(device, &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS) 
        RUN_TIME_ERROR("failed to allocate texture image memory!");
    
    
    if (vkBindImageMemory(device, textureImage, textureImageMemory, 0) != VK_SUCCESS)
        RUN_TIME_ERROR("Error binding texture image memmory");

    // create image view
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = textureImage;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &createInfo, nullptr, &textureImageView) != VK_SUCCESS)
        RUN_TIME_ERROR("failed to create texture image views!");

    //create staging buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imageSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS) 
        RUN_TIME_ERROR("failed to create staging buffer for texture image!");
    

    VkMemoryRequirements bufferMemoryRequirements;
    vkGetBufferMemoryRequirements(device, stagingBuffer, &bufferMemoryRequirements);

    memoryTypeIndex = -1;
    uint32_t bit = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
        if ((memoryRequirements.memoryTypeBits & (1 << i)) &&
            ((memoryProperties.memoryTypes[i].propertyFlags & bit) == bit)) {
                memoryTypeIndex = i;
                break;
            }
    }

    VkMemoryAllocateInfo buffAllocInfo{};
    buffAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    buffAllocInfo.allocationSize = bufferMemoryRequirements.size;
    buffAllocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(device, &buffAllocInfo, nullptr, &stagingBufferMemory) != VK_SUCCESS) 
        RUN_TIME_ERROR("failed to allocate staging buffer memory for texture image!");
    

    if (vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0) != VK_SUCCESS)
        RUN_TIME_ERROR("failed to bind staging buffer memory for texture image!");

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, grassTextureImage.image, static_cast<size_t>(imageSize));
    vkUnmapMemory(device, stagingBufferMemory);

    stbi_image_free(grassTextureImage.image);

    //sampler
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) 
        RUN_TIME_ERROR("failed to create texture sampler!");
    

 
}

void VulkanApp::createDescriptorSetLayout() 
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 0;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 1;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {samplerLayoutBinding, uboLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) 
        RUN_TIME_ERROR("failed to create descriptor set layout!");

}

void VulkanApp::createDescriptorPool() 
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(screenBufferResources.swapChainImages.size());
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(screenBufferResources.swapChainImages.size());

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(screenBufferResources.swapChainImages.size());

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) 
        RUN_TIME_ERROR("failed to create descriptor pool!");
}

void VulkanApp::createDescriptorSets() 
{
    std::vector<VkDescriptorSetLayout> layouts(screenBufferResources.swapChainImages.size(), descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(screenBufferResources.swapChainImages.size());
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(screenBufferResources.swapChainImages.size());
    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) 
        RUN_TIME_ERROR("failed to allocate descriptor sets!");
    

    for (size_t i = 0; i < screenBufferResources.swapChainImages.size(); i++) {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureImageView;
        imageInfo.sampler = textureSampler;

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pImageInfo = &imageInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void VulkanApp::copyVertices2GPU()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuff;
    if (vkAllocateCommandBuffers(device, &allocInfo, &cmdBuff) != VK_SUCCESS)
        RUN_TIME_ERROR("copyVertices2GPU: failed to allocate command buffer!");

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; 
    
    vkBeginCommandBuffer(cmdBuff, &beginInfo); 
    vkCmdUpdateBuffer(cmdBuff, vertexBuffer, 0, vertices.size() * sizeof(float), vertices.data());
    vkCmdUpdateBuffer(cmdBuff, idxBuffer, 0, vertIdxs.size() * sizeof(uint16_t), vertIdxs.data());

    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cmdBuff);
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = { static_cast<unsigned int>(grassTextureImage.w), static_cast<unsigned int>(grassTextureImage.h), 1 };

    vkCmdCopyBufferToImage(cmdBuff, stagingBuffer, textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, cmdBuff);
    
    vkEndCommandBuffer(cmdBuff);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1; 
    submitInfo.pCommandBuffers = &cmdBuff;
                                         
    VkFence fence;
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    if (vkCreateFence(device, &fenceCreateInfo, NULL, &fence) != VK_SUCCESS)
        RUN_TIME_ERROR("copyVertices2GPU: error creating fense");

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
        RUN_TIME_ERROR("copyVertices2GPU: submit failed");

    if (vkWaitForFences(device, 1, &fence, VK_TRUE, 100000000000) != VK_SUCCESS)
        RUN_TIME_ERROR("copyVertices2GPU: fence wait failed");

    vkDestroyFence(device, fence, NULL);
    vkFreeCommandBuffers(device, commandPool, 1, &cmdBuff);
}

void VulkanApp::drawFrame()
{
    vkWaitForFences(device, 1, &syncObj.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &syncObj.inFlightFences[currentFrame]);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, screenBufferResources.swapChain, UINT64_MAX, syncObj.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    
    VkSemaphore waitSemaphores[] = { syncObj.imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    updateUniformBuffer(imageIndex);
    
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = { syncObj.renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, syncObj.inFlightFences[currentFrame]) != VK_SUCCESS)
        RUN_TIME_ERROR("drawFrame: failed to submit draw command buffer!");
    
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores= signalSemaphores;

    VkSwapchainKHR swapChains[] = { screenBufferResources.swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    
    vkQueuePresentKHR(presentQueue, &presentInfo);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanApp::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    
    UniformBufferObject ubo;
    //ubo.time = 2;//std::rand() % 10;
    ubo.proj = glm::perspective(glm::radians(45.0f), 
        screenBufferResources.swapChainExtent.width / (float) screenBufferResources.swapChainExtent.height, 0.1f, 10.0f);
    /*for(int i =0; i < 4; i++){
        for(int j = 0; j < 4; j++)
        {
            std::cerr << ubo.proj[i][j] << " ";
        }
        std::cerr << '\n';
    }*/
    ubo.proj[1][1] *= -1;
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 0.1f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    void* data;
    vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
}

void VulkanApp::initDebugReportCallback()
{
    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    createInfo.pfnCallback = debugReportCallbackFn;

    auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (vkCreateDebugReportCallbackEXT == nullptr)
        RUN_TIME_ERROR("Could not load vkCreateDebugReportCallbackEXT");

    if (vkCreateDebugReportCallbackEXT(instance, &createInfo, NULL, &debugReportCallback) != VK_SUCCESS)
        RUN_TIME_ERROR("You were the Chosen One! It was said that you would destroy the Sith, not join them. bring balance to the force, not leave it in darkness.");
}

VkShaderModule VulkanApp::createShaderModule(const std::vector<uint32_t>& code)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size() * sizeof(uint32_t);
    createInfo.pCode = code.data();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        RUN_TIME_ERROR("createShaderModule: failed to create shader module!");

    return shaderModule;
}

VkDevice& VulkanApp::operator()()
{
    return device;
}

void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkCommandBuffer commandBuffer) 
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        RUN_TIME_ERROR("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
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

void loadShaderModule(const char* filename, std::vector<uint32_t>& data)
{
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL){
        std::string errorMsg = std::string("ReadFile, can't open file ") + std::string(filename);
        RUN_TIME_ERROR(errorMsg.c_str());
    }

    fseek(fp, 0, SEEK_END);
    int filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int filesizePadded = long(ceil(filesize / 4.0)) * 4;

    data.resize(filesizePadded / 4);
    char *str = (char*)data.data();
    fread(str, filesize, sizeof(char), fp);
    fclose(fp);
    for (int i = filesize; i < filesizePadded; i++) {
        str[i] = 0;
    }
}


