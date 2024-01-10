#pragma once

#include <optional>

namespace Reinkan::Graphics
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsAndComputeFamily;
        std::optional<uint32_t> computeOnlyFamily;
        std::optional<uint32_t> presentFamily;

        bool IsComplete() 
        {
            return graphicsAndComputeFamily.has_value() 
                && presentFamily.has_value() 
                && computeOnlyFamily.has_value();
        }
    };

    struct SwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
}
