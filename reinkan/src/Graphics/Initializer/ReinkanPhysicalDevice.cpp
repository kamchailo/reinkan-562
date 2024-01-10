#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include <set>

namespace Reinkan::Graphics
{
    void ReinkanApp::PickPhysicalDevice() 
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(appInstance, &deviceCount, nullptr);

        if (deviceCount == 0) 
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(appInstance, &deviceCount, physicalDevices.data());

#define REINKAN_DISCRETE_GPU_REQUIRED00
#ifdef REINKAN_DISCRETE_GPU_REQUIRED
        for (auto physicalDevice : physicalDevices)
        {
            VkPhysicalDeviceProperties GPUproperties;
            vkGetPhysicalDeviceProperties(physicalDevice, &GPUproperties);

            // Get the GPU's extension list;  Another two-step list retrieval procedure:
            unsigned int extCount;
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);
            std::vector<VkExtensionProperties> extensionProperties(extCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr,
                &extCount, extensionProperties.data());

            printf("- Available Physical Device: %p %s\n", physicalDevice, GPUproperties.deviceName);

            bool satisfied = false;
            if (GPUproperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                for (auto reqExtension : reqDeviceExtensions)
                {
                    satisfied = false;
                    for (auto& deviceExtension : extensionProperties)
                    {
                        if (reqExtension == deviceExtension.extensionName)
                        {
                            satisfied = true;
                            break;
                        }
                    }

                    if (!satisfied)
                    {
                        break;
                    }
                }
                appPhysicalDevice = physicalDevice;

                printf("- - Chosen appPhysicalDevice: %p %s\n", appPhysicalDevice, GPUproperties.deviceName);
            }
            // Furthur Implementation to rank all physical device.
        }
#else   
        for (const auto& physicalDevice : physicalDevices)
        {
            VkPhysicalDeviceProperties GPUproperties;
            vkGetPhysicalDeviceProperties(physicalDevice, &GPUproperties);
            printf("- Available Physical Device: %p %s\n", physicalDevice, GPUproperties.deviceName);
            if (IsDeviceSuitable(physicalDevice) && GPUproperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                appPhysicalDevice = physicalDevice;
                
                // MultiSampling
                appMsaaSamples = GetMaxUsableSampleCount();

                std::printf("- - Chosen appPhysicalDevice: %p %s\n", appPhysicalDevice, GPUproperties.deviceName);
                break;
            }
        }   
#endif // REINKAN_DISCRETE_GPU_REQUIRED

        if (appPhysicalDevice == VK_NULL_HANDLE) 
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    bool ReinkanApp::IsDeviceSuitable(VkPhysicalDevice device) 
    {
        QueueFamilyIndices indices = FindQueueFamilies(device);

        bool extensionsSupported = CheckDeviceExtensionSupport(device);

        bool swapchainAdequate = false;
        if (extensionsSupported) 
        {
            SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(device);
            swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.IsComplete() && extensionsSupported && swapchainAdequate && supportedFeatures.samplerAnisotropy;
    }
    

    QueueFamilyIndices ReinkanApp::FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) 
        {
            // Dedicated compute queue
            if (!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
            {
                indices.computeOnlyFamily = i;
            }

            if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
                && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))
            {
                indices.graphicsAndComputeFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, appSurface, &presentSupport);

            if (presentSupport) 
            {
                indices.presentFamily = i;
            }

            if (indices.IsComplete()) 
            {
                break;
            }

            i++;
        }

        return indices;
    }

    bool ReinkanApp::CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(reqDeviceExtensions.begin(), reqDeviceExtensions.end());

        for (const auto& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }
}