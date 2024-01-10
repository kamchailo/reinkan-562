#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include "stb_image/stb_image.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::AddPyramidalPath(std::string const& path)
    {
        appPyramidalPaths.push_back(path);
    }

    void ReinkanApp::BindPyramidalMap(std::vector<std::string> const& filePaths)
    {
        appPyramidalImageWraps.resize(filePaths.size());
        for (int i =0; i < filePaths.size(); ++i)
        {

            //appPyramidalImageWraps[i] = CreateTextureImageWrap(filePaths[i]);
            
            int texWidth, texHeight, texChannels;
            stbi_uc* pixels = stbi_load(filePaths[i].c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
            VkDeviceSize imageSize = texWidth * texHeight * 4;

            if (!pixels) {
                throw std::runtime_error("failed to load texture image!");
            }

            uint32_t mipLevels = 1;

            BufferWrap stagingBufferWrap = CreateBufferWrap(imageSize,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            void* data;
            vkMapMemory(appDevice, stagingBufferWrap.memory, 0, imageSize, 0, &data);
            memcpy(data, pixels, static_cast<size_t>(imageSize));
            vkUnmapMemory(appDevice, stagingBufferWrap.memory);

            stbi_image_free(pixels);

            appPyramidalImageWraps[i] = CreateImageWrap(texWidth,
                texHeight,
                VK_FORMAT_R8G8B8A8_SRGB,                                         // Image Format
                VK_IMAGE_TILING_OPTIMAL,                                         // Image Tilling
                VK_IMAGE_USAGE_TRANSFER_SRC_BIT  // Added for creating mipmaps which is considers a transfer operation
                | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,  // Image Usage
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,                             // Memory Property
                mipLevels);                                                      // Mip Levels

            TransitionImageLayout(appPyramidalImageWraps[i].image,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                mipLevels);

            CopyBufferToImage(stagingBufferWrap.buffer,
                appPyramidalImageWraps[i].image,
                static_cast<uint32_t>(texWidth),
                static_cast<uint32_t>(texHeight));

            stagingBufferWrap.Destroy(appDevice);
   
            TransitionImageLayout(appPyramidalImageWraps[i].image,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                mipLevels);
                
            appPyramidalImageWraps[i].imageView = CreateImageView(appPyramidalImageWraps[i].image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
            appPyramidalImageWraps[i].sampler = CreateNearestImageSampler();
            appPyramidalImageWraps[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
    }
}