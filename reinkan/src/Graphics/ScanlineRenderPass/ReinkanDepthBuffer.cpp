#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::CreateSwapchainDepthResource()
    {
        VkFormat depthFormat = FindDepthFormat();

        appSwapchainDepthImageWrap = CreateImageWrap(appSwapchainExtent.width, 
                                                    appSwapchainExtent.height, 
                                                    depthFormat, 
                                                    VK_IMAGE_TILING_OPTIMAL, 
                                                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT 
                                                    | VK_IMAGE_USAGE_SAMPLED_BIT,
                                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                    1,
                                                    appMsaaSamples);

        appSwapchainDepthImageWrap.imageView = CreateImageView(appSwapchainDepthImageWrap.image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
        
        TransitionImageLayout(appSwapchainDepthImageWrap.image, 
                              depthFormat, 
                              VK_IMAGE_LAYOUT_UNDEFINED, 
                              VK_IMAGE_LAYOUT_GENERAL);

        appSwapchainDepthImageWrap.sampler = CreateImageSampler();
        appSwapchainDepthImageWrap.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    }

    VkFormat ReinkanApp::FindDepthFormat()
    {
        return FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
                                    VK_IMAGE_TILING_OPTIMAL,
                                    VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    VkFormat ReinkanApp::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates) 
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(appPhysicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) 
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
            {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    bool ReinkanApp::HasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }
}