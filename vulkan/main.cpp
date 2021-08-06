#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <tuple>
#include <algorithm>

static void error_callback(int code, const char *description)
{
    std::cerr << "glfw error code: " << code << " (" << description << ")" << std::endl;
}

static void render(GLFWwindow *window)
{
}

static VkInstance createInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan ditty";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;

    // set VK_LOADER_DEBUG=all to debug this
    VkInstance instance;
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

    return instance;
}

static VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow *window)
{
    VkSurfaceKHR surface;
    VkResult err = glfwCreateWindowSurface(instance, window, NULL, &surface);
    if (err)
    {
        throw std::runtime_error("failed to create surface!");
    }
    return surface;
}

static VkPhysicalDevice getPhysicalDevice(VkInstance instance)
{
    VkPhysicalDevice physicalDevice;
    uint32_t deviceCount = 1;
    VkResult res = vkEnumeratePhysicalDevices(instance, &deviceCount, &physicalDevice);
    if (res != VK_SUCCESS && res != VK_INCOMPLETE)
    {
        throw std::runtime_error("Enumerating physical devices failed");
    }
    if (0 == deviceCount)
    {
        throw std::runtime_error("No physical devices that support Vulkan");
    }
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    uint32_t supportedVersion[] =
    {
        VK_VERSION_MAJOR(deviceProperties.apiVersion),
        VK_VERSION_MINOR(deviceProperties.apiVersion),
        VK_VERSION_PATCH(deviceProperties.apiVersion)
    };

    std::cout << "Physical device supports version " << supportedVersion[0] << "." << supportedVersion[1] << "." << supportedVersion[2] << std::endl;

    return physicalDevice;
}

static void checkSwapChainSupport(VkPhysicalDevice physicalDevice)
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    if (0 == extensionCount)
    {
        throw std::runtime_error("Physical device doesn't support any extensions");
    }

    std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, deviceExtensions.data());

    for (const auto& extension : deviceExtensions)
    {
        if (0 == strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
        {
            std::cout << "Physical device supports swap chains" << std::endl;
            return;
        }
    }

    throw std::runtime_error("Physical device doesn't support swap chains");
}

static std::tuple<uint32_t, uint32_t> getQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    if (0 == queueFamilyCount)
    {
        throw std::runtime_error("Physical device has no queue families");
    }

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    std::cout << "Physical device has " << queueFamilyCount << " queue families" << std::endl;

    bool foundGraphicsQueueFamily = false;
    bool foundPresentQueueFamily = false;
    uint32_t graphicsQueueFamily;
    uint32_t presentQueueFamily;

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, windowSurface, &presentSupport);

        if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsQueueFamily = i;
            foundGraphicsQueueFamily = true;

            if (presentSupport)
            {
                presentQueueFamily = i;
                foundPresentQueueFamily = true;
                break;
            }
        }

        if (!foundPresentQueueFamily && presentSupport)
        {
            presentQueueFamily = i;
            foundPresentQueueFamily = true;
        }
    }

    if (foundGraphicsQueueFamily)
    {
        std::cout << "Queue family #" << graphicsQueueFamily << " supports graphics" << std::endl;

        if (foundPresentQueueFamily)
        {
            std::cout << "Queue family #" << presentQueueFamily << " supports presentation" << std::endl;
        }
        else
        {
            throw std::runtime_error("Could not find a valid queue family with present support");
        }
    }
    else
    {
        throw std::runtime_error("Could not find a valid queue family with graphics support");
    }

    return std::make_tuple(graphicsQueueFamily, presentQueueFamily);
}

static std::tuple<VkDevice, VkQueue, VkQueue> createLogicalDevice(VkPhysicalDevice physicalDevice, const uint32_t graphicsQueueFamily, const uint32_t presentQueueFamily)
{
    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queueCreateInfo[2] = {};

    queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[0].queueFamilyIndex = graphicsQueueFamily;
    queueCreateInfo[0].queueCount = 1;
    queueCreateInfo[0].pQueuePriorities = &queuePriority;

    queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[0].queueFamilyIndex = presentQueueFamily;
    queueCreateInfo[0].queueCount = 1;
    queueCreateInfo[0].pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfo;

    if (graphicsQueueFamily == presentQueueFamily)
    {
        deviceCreateInfo.queueCreateInfoCount = 1;
    }
    else
    {
        deviceCreateInfo.queueCreateInfoCount = 2;
    }

    const char* deviceExtensions = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    deviceCreateInfo.enabledExtensionCount = 1;
    deviceCreateInfo.ppEnabledExtensionNames = &deviceExtensions;

    VkDevice device;
    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device");
    }

    std::cout << "Created logical device" << std::endl;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentQueueFamily, 0, &presentQueue);

    std::cout << "Acquired graphics and presentation queues" << std::endl;

    return std::make_tuple(device, graphicsQueue, presentQueue);
}

static VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
    }

    for (const auto& availableSurfaceFormat : availableFormats)
    {
        if (availableSurfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM)
        {
            return availableSurfaceFormat;
        }
    }

    return availableFormats[0];
}

static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
    if (surfaceCapabilities.currentExtent.width == -1)
    {
        VkExtent2D swapChainExtent = {};

        swapChainExtent.width = std::min(std::max(640u, surfaceCapabilities.minImageExtent.width), surfaceCapabilities.maxImageExtent.width);
        swapChainExtent.height = std::min(std::max(480u, surfaceCapabilities.minImageExtent.height), surfaceCapabilities.maxImageExtent.height);

        return swapChainExtent;
    }
    else
    {
        return surfaceCapabilities.currentExtent;
    }
}

VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> presentModes)
{
    for (const auto& presentMode : presentModes)
    {
        if (VK_PRESENT_MODE_MAILBOX_KHR == presentMode)
        {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static std::tuple<VkSwapchainKHR, std::vector<VkImage>> createSwapChain(VkSurfaceKHR windowSurface, VkPhysicalDevice physicalDevice, VkDevice device)
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, windowSurface, &surfaceCapabilities) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to acquire presentation surface capabilities");
    }

    // Find supported surface formats
    uint32_t formatCount;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &formatCount, nullptr) != VK_SUCCESS || 0 == formatCount)
    {
        throw std::runtime_error("failed to get number of supported surface formats");
    }

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &formatCount, surfaceFormats.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to get supported surface formats");
    }

    uint32_t presentModeCount;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &presentModeCount, nullptr) != VK_SUCCESS || 0 == presentModeCount)
    {
        throw std::runtime_error("Failed to get number of supported presentation modes");
    }

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &presentModeCount, presentModes.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to get supported presentation modes");
    }

    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount != 0 && imageCount > surfaceCapabilities.maxImageCount)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    std::cout << "Using " << imageCount << " images for swap chain" << std::endl;

    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(surfaceFormats);

    VkExtent2D swapChainExtent = chooseSwapExtent(surfaceCapabilities);

    if (!(surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
    {
        std::cerr << "swap chain image does not support VK_IMAGE_TRANSFER_DST usage" << std::endl;
        //exit(1);
    }

    // Determine transformation to use (preferring no transform)
    VkSurfaceTransformFlagBitsKHR surfaceTransform;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        surfaceTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        surfaceTransform = surfaceCapabilities.currentTransform;
    }

    VkPresentModeKHR presentMode = choosePresentMode(presentModes);

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = windowSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = swapChainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
    createInfo.preTransform = surfaceTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapChain;
    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain");
    }
    else
    {
        std::cout << "Created swap chain" << std::endl;
    }

    uint32_t actualImageCount = 0;
    if (vkGetSwapchainImagesKHR(device, swapChain, &actualImageCount, nullptr) != VK_SUCCESS || 0 == actualImageCount)
    {
        throw std::runtime_error("Failed to acquire number of swap chain images");
    }

    std::vector<VkImage> swapChainImages;
    swapChainImages.resize(actualImageCount);

    if (vkGetSwapchainImagesKHR(device, swapChain, &actualImageCount, swapChainImages.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to acquire swap chain images");
    }

    std::cout << "Acquired swap chain images" << std::endl;

    return std::make_tuple(swapChain, swapChainImages);
}

int main()
{
    if (!glfwInit())
    {
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);

    std::cout << "Running against GLFW " << major << "." << minor << "." << revision << std::endl;

    if (!glfwVulkanSupported())
    {
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(640, 480, "Vulkan ditty", nullptr, nullptr);

    VkInstance instance = createInstance();
    VkSurfaceKHR surface = createSurface(instance, window);
    VkPhysicalDevice physicalDevice = getPhysicalDevice(instance);
    checkSwapChainSupport(physicalDevice);
    auto [graphicsQueueFamily, presentQueueFamily] = getQueueFamilies(physicalDevice, surface);
    auto [device, graphicsQueue, presentQueue] = createLogicalDevice(physicalDevice, graphicsQueueFamily, presentQueueFamily);
    auto [swapChain, swapChainImages] = createSwapChain(surface, physicalDevice, device);

    while (!glfwWindowShouldClose(window))
    {
        render(window);

        glfwPollEvents();
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
