#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp

namespace Reinkan::Graphics
{
    void ReinkanApp::CreateSwapchain()
    {
        SwapchainSupportDetails swapChainSupport = QuerySwapchainSupport(appPhysicalDevice);

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapchainPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = ChooseSwapchainExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 
            && imageCount > swapChainSupport.capabilities.maxImageCount) 
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = appSurface;

        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        QueueFamilyIndices indices = FindQueueFamilies(appPhysicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsAndComputeFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsAndComputeFamily != indices.presentFamily) 
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        // Almost all GPU has the same queue that support both.
        else 
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        swapchainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;

        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(appDevice, &swapchainCreateInfo, nullptr, &appSwapchain) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        // Get VkImage from Swaphcain KHR, Developer does not create VkImage for swapchain manually.
        vkGetSwapchainImagesKHR(appDevice, appSwapchain, &imageCount, nullptr);
        appSwapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(appDevice, appSwapchain, &imageCount, appSwapchainImages.data());

        appSwapchainImageFormat = surfaceFormat.format;
        appSwapchainExtent = extent;
    }

    SwapchainSupportDetails ReinkanApp::QuerySwapchainSupport(VkPhysicalDevice device)
    {
        SwapchainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, appSurface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, appSurface, &formatCount, nullptr);
        if (formatCount != 0) 
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, appSurface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, appSurface, &presentModeCount, nullptr);
        if (presentModeCount != 0) 
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, appSurface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR ReinkanApp::ChooseSwapchainSurfaceFormat(std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) 
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB 
                && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
            {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR ReinkanApp::ChooseSwapchainPresentMode(std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes) 
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D ReinkanApp::ChooseSwapchainExtent(VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(appWindow, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void ReinkanApp::CreateSwapchainImageViews()
    {
        appSwapchainImageViews.resize(appSwapchainImages.size());

        for (size_t i = 0; i < appSwapchainImages.size(); i++)
        {
            appSwapchainImageViews[i] = CreateImageView(appSwapchainImages[i], appSwapchainImageFormat);
        }
    }

    void ReinkanApp::RecreateSwapchain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(appWindow, &width, &height);

        while (width == 0 || height == 0) 
        {
            glfwGetFramebufferSize(appWindow, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(appDevice);

        CleanupSwapchain();

        CreateSwapchain();
        CreateSwapchainImageViews();
        CreateSwapchainColorResources();
        CreateSwapchainDepthResource();
        CreateSwapchainFrameBuffers();

        // Have to recreate because shared depth
        CreateShadowFrameBuffers();
        // Recreate Scanline FrameBuffers
        CreateScanlineFrameBuffers();
        // Recreate VLight FrameBuffers
        CreateVLightFrameBuffers();
        // Recreate DeferredLight FrameBuffers
        CreateDeferredLightFrameBuffers();

        // Rebind Descriptor for Scanline
        appScanlineDescriptorWrap.Write(appDevice, 4, appShadowMapImageWraps, MAX_FRAMES_IN_FLIGHT);

        // Rebind Compute Shadow Blur
        appShadowBlurDescriptorWrap.Write(appDevice, 1, appShadowMapImageWraps, MAX_FRAMES_IN_FLIGHT);

        // Rebind Descriptor for VLight
        appVLightDescriptorWrap.Write(appDevice, 1, appShadowMapImageWraps, MAX_FRAMES_IN_FLIGHT);
        appVLightDescriptorWrap.Write(appDevice, 2, appScanlinePositionImageWraps, MAX_FRAMES_IN_FLIGHT);

        // Rebind Descriptor for DeferredLight
        uint32_t bindingIndex = 0;
        appDeferredLightDescriptorWrap.Write(appDevice, bindingIndex++, appScanlineUBO);
        appDeferredLightDescriptorWrap.Write(appDevice, bindingIndex++, appShadowMapImageWraps, MAX_FRAMES_IN_FLIGHT);

        // 3 - Light Objects
        if (appLightObjects.size() > 0)
        {
            appDeferredLightDescriptorWrap.Write(appDevice, bindingIndex++, appClusteredGlobalLights.buffer, MAX_FRAMES_IN_FLIGHT);
        }

        // Color Attachment
        appDeferredLightDescriptorWrap.Write(appDevice, bindingIndex++, appScanlineImageWraps, MAX_FRAMES_IN_FLIGHT);
        // Position Attachment
        appDeferredLightDescriptorWrap.Write(appDevice, bindingIndex++, appScanlinePositionImageWraps, MAX_FRAMES_IN_FLIGHT);
        // Normal Attachment
        appDeferredLightDescriptorWrap.Write(appDevice, bindingIndex++, appScanlineNormalImageWraps, MAX_FRAMES_IN_FLIGHT);
        // Specular Attachment
        appDeferredLightDescriptorWrap.Write(appDevice, bindingIndex++, appScanlineSpecularImageWraps, MAX_FRAMES_IN_FLIGHT);

        // Rebind Descriptor for Post Processing
        bindingIndex = 0;
        appPostDescriptorWrap.Write(appDevice, bindingIndex++, appScanlineImageWraps, MAX_FRAMES_IN_FLIGHT);
        appPostDescriptorWrap.Write(appDevice, bindingIndex++, appShadowMapImageWraps, MAX_FRAMES_IN_FLIGHT);
        appPostDescriptorWrap.Write(appDevice, bindingIndex++, appVLightingRenderTargetImageWraps, MAX_FRAMES_IN_FLIGHT);
        appPostDescriptorWrap.Write(appDevice, bindingIndex++, appScanlinePositionImageWraps, MAX_FRAMES_IN_FLIGHT);
        appPostDescriptorWrap.Write(appDevice, bindingIndex++, appScanlineNormalImageWraps, MAX_FRAMES_IN_FLIGHT);
        appPostDescriptorWrap.Write(appDevice, bindingIndex++, appScanlineSpecularImageWraps, MAX_FRAMES_IN_FLIGHT);
        appPostDescriptorWrap.Write(appDevice, bindingIndex++, appDeferredLightingRenderTargetImageWraps, MAX_FRAMES_IN_FLIGHT);
    }

    void ReinkanApp::CleanupSwapchain()
    {
        for (size_t i = 0; i < appSwapchainFramebuffers.size(); i++) 
        {
            vkDestroyFramebuffer(appDevice, appSwapchainFramebuffers[i], nullptr);
        }

        for (size_t i = 0; i < appSwapchainImageViews.size(); i++) 
        {
            vkDestroyImageView(appDevice, appSwapchainImageViews[i], nullptr);
        }

        appMsaaImageWrap.Destroy(appDevice);

        appSwapchainDepthImageWrap.Destroy(appDevice);

        vkDestroySwapchainKHR(appDevice, appSwapchain, nullptr);
        
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            // Scanline ImageWrap
            appScanlineImageWraps[i].Destroy(appDevice);

            // ScanlinePosition ImageWrap
            appScanlinePositionImageWraps[i].Destroy(appDevice);

            // ScanlineNormal ImageWrap
            appScanlineNormalImageWraps[i].Destroy(appDevice);

            // ScanlineSpecular ImageWrap
            appScanlineSpecularImageWraps[i].Destroy(appDevice);

            // Scanline FrameBuffers
            vkDestroyFramebuffer(appDevice, appScanlineFrameBuffers[i], nullptr);

            // Shadow
            appShadowMapImageWraps[i].Destroy(appDevice);
            vkDestroyFramebuffer(appDevice, appShadowFrameBuffers[i], nullptr);

            // VLight
            appVLightingRenderTargetImageWraps[i].Destroy(appDevice);
            vkDestroyFramebuffer(appDevice, appVLightFrameBuffers[i], nullptr);

            // DeferredLight
            appDeferredLightingRenderTargetImageWraps[i].Destroy(appDevice);
            vkDestroyFramebuffer(appDevice, appDeferredLightFrameBuffers[i], nullptr);
        }

        appScanlineImageWraps.clear();
        appScanlinePositionImageWraps.clear();
        appScanlineNormalImageWraps.clear();
        appScanlineSpecularImageWraps.clear();
        appShadowMapImageWraps.clear();
        appVLightingRenderTargetImageWraps.clear();
        appDeferredLightingRenderTargetImageWraps.clear();
    }
}