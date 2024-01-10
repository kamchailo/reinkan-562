#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "Graphics/Structure/ImageWrap.h"
#include "Graphics/Structure/BufferWrap.h"

namespace Reinkan::Graphics
{
    class DescriptorWrap
    {
    public:
        std::vector<VkDescriptorSetLayoutBinding> bindingTable;

        VkDescriptorSetLayout descriptorSetLayout;

        VkDescriptorPool descriptorPool;

        std::vector<VkDescriptorSet> descriptorSets;

        void SetBindings(const VkDevice device, std::vector<VkDescriptorSetLayoutBinding> bindings, const uint32_t maxSets);

        void Destroy(VkDevice device);

        /*
        * Need to change how to write
        * Has vector to store all write data instead of vkUpdateDescriptorSets by calling Write
        * then call vkUpdateDescriptorSets once with that vector
        */
        void Write(VkDevice& device, uint32_t index, const VkBuffer& buffer, const uint32_t maxSets);

        void Write(VkDevice& device, uint32_t index, const std::vector<BufferWrap>& bufferWraps);

        void Write(VkDevice& device, uint32_t index, const ImageWrap& texture, uint32_t maxSets);

        void Write(VkDevice& device, uint32_t index, const std::vector<ImageWrap>& textures, const uint32_t maxSets);

        void WriteFromFrameBuffers(VkDevice& device, uint32_t index, const std::vector<ImageWrap>& textures);

        void WriteFromFrameBuffer(VkDevice& device, uint32_t index, const ImageWrap& texture);
    };

}