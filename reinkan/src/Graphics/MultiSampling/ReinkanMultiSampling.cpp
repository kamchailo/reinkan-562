#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    /// <summary>
    /// Under Construction
    /// <para>Return only VK_SAMPLE_COUNT_1_BIT</para>
    /// <para>To support MultiPass</para>
    /// </summary>
    /// <returns>VK_SAMPLE_COUNT_1_BIT</returns>
    VkSampleCountFlagBits ReinkanApp::GetMaxUsableSampleCount()
    {
        return VK_SAMPLE_COUNT_1_BIT;

        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(appPhysicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        
        if (counts & VK_SAMPLE_COUNT_64_BIT)    { return VK_SAMPLE_COUNT_64_BIT; }

        if (counts & VK_SAMPLE_COUNT_32_BIT)    { return VK_SAMPLE_COUNT_32_BIT; }

        if (counts & VK_SAMPLE_COUNT_16_BIT)    { return VK_SAMPLE_COUNT_16_BIT; }

        if (counts & VK_SAMPLE_COUNT_8_BIT)     { return VK_SAMPLE_COUNT_8_BIT; }

        if (counts & VK_SAMPLE_COUNT_4_BIT)     { return VK_SAMPLE_COUNT_4_BIT; }

        if (counts & VK_SAMPLE_COUNT_2_BIT)     { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    void ReinkanApp::CreateSwapchainColorResources()
    {
        VkFormat colorFormat = appSwapchainImageFormat;

        appMsaaImageWrap =  CreateImageWrap(appSwapchainExtent.width, 
                                            appSwapchainExtent.height, 
                                            colorFormat, 
                                            VK_IMAGE_TILING_OPTIMAL, 
                                            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                            1,
                                            appMsaaSamples);

        appMsaaImageWrap.imageView = CreateImageView(appMsaaImageWrap.image, 
                                                     colorFormat, 
                                                     VK_IMAGE_ASPECT_COLOR_BIT, 
                                                     1);

    }
}