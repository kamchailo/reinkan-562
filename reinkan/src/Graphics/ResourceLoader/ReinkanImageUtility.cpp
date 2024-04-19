#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    ImageWrap ReinkanApp::CreateImageWrap(uint32_t width,
                                          uint32_t height,
                                          VkFormat format,
                                          VkImageTiling tiling,
                                          VkImageUsageFlags usage,
                                          VkMemoryPropertyFlags properties,
                                          uint32_t  mipLevels,
                                          VkSampleCountFlagBits numSamples)
    {
        ImageWrap imageWrap;
        
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width = width;
        imageCreateInfo.extent.height = height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = mipLevels;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = format;
        imageCreateInfo.tiling = tiling;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = usage;
        imageCreateInfo.samples = numSamples;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(appDevice, &imageCreateInfo, nullptr, &imageWrap.image) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(appDevice, imageWrap.image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(appDevice, &allocInfo, nullptr, &imageWrap.memory) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        // Improve by use offset when binding Image Memory
        // - alloc total size of every texture (totalSize += memRequirements.size)
        // - bind with offset in the same memory block
        vkBindImageMemory(appDevice, imageWrap.image, imageWrap.memory, 0);
        
        imageWrap.imageView = VK_NULL_HANDLE;
        imageWrap.sampler = VK_NULL_HANDLE;

        return imageWrap;
    }

    ImageWrap ReinkanApp::CreateImage3DWrap(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t mipLevels, VkSampleCountFlagBits numSamples)
    {
        ImageWrap imageWrap;

        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_3D;
        imageCreateInfo.extent.width = width;
        imageCreateInfo.extent.height = height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = mipLevels;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = format;
        imageCreateInfo.tiling = tiling;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = usage;
        imageCreateInfo.samples = numSamples;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(appDevice, &imageCreateInfo, nullptr, &imageWrap.image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(appDevice, imageWrap.image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(appDevice, &allocInfo, nullptr, &imageWrap.memory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        // Improve by use offset when binding Image Memory
        // - alloc total size of every texture (totalSize += memRequirements.size)
        // - bind with offset in the same memory block
        vkBindImageMemory(appDevice, imageWrap.image, imageWrap.memory, 0);

        imageWrap.imageView = VK_NULL_HANDLE;
        imageWrap.sampler = VK_NULL_HANDLE;

        return imageWrap;
    }

    VkImageView ReinkanApp::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlagBits aspect, uint32_t  mipLevels)
    {
        VkImageView imageView;

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspect;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(appDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    VkImageView ReinkanApp::CreateImage3DView(VkImage image, VkFormat format, VkImageAspectFlagBits aspect, uint32_t mipLevels)
    {
        VkImageView imageView;

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspect;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(appDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    VkSampler ReinkanApp::CreateImageSampler()
    {
        VkSampler sampler;

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;

        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        samplerInfo.anisotropyEnable = VK_TRUE;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(appPhysicalDevice, &properties);
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(appDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create texture sampler!");
        }

        return sampler;
    }

    void ReinkanApp::CreateImageLayoutBarrier(VkCommandBuffer commandBuffer, 
                                                VkImage image, 
                                                VkImageLayout oldImageLayout, 
                                                VkImageLayout newImageLayout, 
                                                VkImageAspectFlags aspectMask)
    {
        VkImageSubresourceRange subresourceRange;
        subresourceRange.aspectMask = aspectMask;
        subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.baseArrayLayer = 0;

        VkImageMemoryBarrier imageMemoryBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        imageMemoryBarrier.oldLayout = oldImageLayout;
        imageMemoryBarrier.newLayout = newImageLayout;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange = subresourceRange;
        imageMemoryBarrier.srcAccessMask = AccessFlagsForImageLayout(oldImageLayout);
        imageMemoryBarrier.dstAccessMask = AccessFlagsForImageLayout(newImageLayout);
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        VkPipelineStageFlags srcStageMask = PipelineStageForLayout(oldImageLayout);
        VkPipelineStageFlags destStageMask = PipelineStageForLayout(newImageLayout);

        vkCmdPipelineBarrier(commandBuffer, srcStageMask, destStageMask, 0,
            0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
    }

    VkAccessFlags ReinkanApp::AccessFlagsForImageLayout(VkImageLayout layout)
    {
        switch (layout)
        {
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            return VK_ACCESS_HOST_WRITE_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return VK_ACCESS_TRANSFER_WRITE_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return VK_ACCESS_TRANSFER_READ_BIT;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return VK_ACCESS_SHADER_READ_BIT;
        default:
            return VkAccessFlags();
        }
    }

    VkPipelineStageFlags ReinkanApp::PipelineStageForLayout(VkImageLayout layout)
    {
        switch (layout)
        {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return VK_PIPELINE_STAGE_TRANSFER_BIT;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;  // Allow queue other than graphic
            // return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;  // Allow queue other than graphic
            // return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            return VK_PIPELINE_STAGE_HOST_BIT;
        case VK_IMAGE_LAYOUT_UNDEFINED:
            return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        default:
            return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }
    }

    VkSampler ReinkanApp::CreateTextureSampler(uint32_t mipLevels)
    {
        VkSampler textureSampler;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(appPhysicalDevice, &properties);

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
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mipLevels);
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(appDevice, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create texture sampler!");
        }

        return textureSampler;
    }

    VkSampler ReinkanApp::CreateNearestImageSampler()
    {
        VkSampler sampler;

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_NEAREST;

        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        samplerInfo.anisotropyEnable = VK_FALSE;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(appPhysicalDevice, &properties);
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(appDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }

        return sampler;
    }

    void ReinkanApp::TransitionImageLayout(VkImage image, 
                                           VkFormat format, 
                                           VkImageLayout oldLayout, 
                                           VkImageLayout newLayout,
                                           uint32_t  mipLevels)
    {
        VkCommandBuffer tempCommandBuffer = BeginTempCommandBuffer();
        {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // To transfer queue family ownership
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // To transfer queue family ownership
            barrier.image = image;

            if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL || format == FindDepthFormat())
            {
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

                if (HasStencilComponent(format)) 
                {
                    barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                }
            }
            else 
            {
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            }

            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = mipLevels;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            VkPipelineStageFlags sourceStage;
            // -------- Barrier --------
            VkPipelineStageFlags destinationStage;

            if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            }
            else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
            {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }
            else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;  // Depth Test before fragment
                // Then write in VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT
            }
            else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }
            else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL)
            {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;  // Depth Test before fragment
                // Then write in VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
            }
            else 
            {
                throw std::invalid_argument("unsupported layout transition!");
            }

            vkCmdPipelineBarrier(tempCommandBuffer,
                                 sourceStage,
                                 destinationStage,
                                 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier );

        }
        EndTempCommandBuffer(tempCommandBuffer);
    }

    void ReinkanApp::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
    {
        VkCommandBuffer tempCommandBuffer = BeginTempCommandBuffer();
        {
            VkBufferImageCopy imageCopyRegion{};
            imageCopyRegion.bufferOffset = 0;
            imageCopyRegion.bufferRowLength = 0;
            imageCopyRegion.bufferImageHeight = 0;

            imageCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageCopyRegion.imageSubresource.mipLevel = 0;
            imageCopyRegion.imageSubresource.baseArrayLayer = 0;
            imageCopyRegion.imageSubresource.layerCount = 1;

            imageCopyRegion.imageOffset = { 0, 0, 0 };
            imageCopyRegion.imageExtent = { width,
                                            height,
                                            1};

            vkCmdCopyBufferToImage(tempCommandBuffer,
                                   buffer,
                                   image,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   1,
                                   &imageCopyRegion);

        }
        EndTempCommandBuffer(tempCommandBuffer);
    }

    void ReinkanApp::CreateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
    {
        // Check if image format supports linear blitting
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(appPhysicalDevice, imageFormat, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) 
        {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = BeginTempCommandBuffer();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++) 
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            // Make sure the image is in VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
            // before generate mipmaps
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer,
                image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            // Can be replace with compute shader 

            // For more advance filtering
            // https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#VkSamplerReductionModeCreateInfo

            // Texel filtering
            // https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#textures-texel-filtering

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (mipWidth > 1)   mipWidth /= 2;
            if (mipHeight > 1)  mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        EndTempCommandBuffer(commandBuffer);
    }

    void ReinkanApp::CmdCopyImage(VkCommandBuffer commandBuffer, 
                                  ImageWrap& srcImage, 
                                  ImageWrap& dstImage,
                                  uint32_t imageWidth,
                                  uint32_t imageHeight)
    {
        VkImageCopy imageCopyRegion{};
        imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.srcSubresource.layerCount = 1;
        imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageCopyRegion.dstSubresource.layerCount = 1;
        imageCopyRegion.extent.width = imageWidth;
        imageCopyRegion.extent.height = imageWidth;
        imageCopyRegion.extent.depth = 1;

        CmdImageLayoutBarrier(commandBuffer, srcImage.image,
                              VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        CmdImageLayoutBarrier(commandBuffer, dstImage.image,
                              VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        vkCmdCopyImage(commandBuffer,
                       srcImage.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       dstImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &imageCopyRegion);

        CmdImageLayoutBarrier(commandBuffer, srcImage.image,
                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);
        CmdImageLayoutBarrier(commandBuffer, dstImage.image,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

    }

    void ReinkanApp::CmdImageLayoutBarrier(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkImageAspectFlags aspectMask)
    {
        VkImageSubresourceRange subresourceRange;
        subresourceRange.aspectMask = aspectMask;
        subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.baseArrayLayer = 0;

        VkImageMemoryBarrier imageMemoryBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        imageMemoryBarrier.oldLayout = oldImageLayout;
        imageMemoryBarrier.newLayout = newImageLayout;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange = subresourceRange;
        imageMemoryBarrier.srcAccessMask = GetAccessFlagFromImageLayout(oldImageLayout);
        imageMemoryBarrier.dstAccessMask = GetAccessFlagFromImageLayout(newImageLayout);
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        VkPipelineStageFlags srcStageMask = GetPipelineStageFromImageLayout(oldImageLayout);
        VkPipelineStageFlags destStageMask = GetPipelineStageFromImageLayout(newImageLayout);

        vkCmdPipelineBarrier(commandBuffer, srcStageMask, destStageMask, 0,
            0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
    }

    VkAccessFlags ReinkanApp::GetAccessFlagFromImageLayout(VkImageLayout layout)
    {
        switch (layout)
        {
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            return VK_ACCESS_HOST_WRITE_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return VK_ACCESS_TRANSFER_WRITE_BIT;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return VK_ACCESS_TRANSFER_READ_BIT;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return VK_ACCESS_SHADER_READ_BIT;
        default:
            return VkAccessFlags{};
        }
    }

    VkPipelineStageFlags ReinkanApp::GetPipelineStageFromImageLayout(VkImageLayout layout)
    {
        switch (layout)
        {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return VK_PIPELINE_STAGE_TRANSFER_BIT;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;  // Allow queue other than graphic
            // return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;  // Allow queue other than graphic
            // return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            return VK_PIPELINE_STAGE_HOST_BIT;
        case VK_IMAGE_LAYOUT_UNDEFINED:
            return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        default:
            return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }
    }


}