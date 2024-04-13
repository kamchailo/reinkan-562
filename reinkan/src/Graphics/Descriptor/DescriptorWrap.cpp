#include "pch.h"
#include "DescriptorWrap.h"

#include <assert.h>

#include "Graphics/Constant/CoreConstant.h"

namespace Reinkan::Graphics
{
	void DescriptorWrap::SetBindings(const VkDevice device, std::vector<VkDescriptorSetLayoutBinding> bindings, const uint32_t maxSets)
	{
        bindingTable = bindings;

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType =  VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.pNext = nullptr;
        layoutCreateInfo.flags = 0;
        layoutCreateInfo.bindingCount = uint32_t(bindingTable.size());
        layoutCreateInfo.pBindings = bindingTable.data();

        if (vkCreateDescriptorSetLayout(device, &layoutCreateInfo, nullptr, &this->descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        // Collect the size required for each descriptorType into a vector of poolSizes
        std::vector<VkDescriptorPoolSize> poolSizes;

        for (auto it = bindingTable.cbegin(); it != bindingTable.cend(); ++it) 
        {
            bool found = false;
            for (auto itpool = poolSizes.begin(); itpool != poolSizes.end(); ++itpool) 
            {
                if (itpool->type == it->descriptorType) 
                {
                    itpool->descriptorCount += it->descriptorCount * maxSets;
                    found = true;
                    break;
                }
            }

            if (!found) 
            {
                VkDescriptorPoolSize poolSize;
                poolSize.type = it->descriptorType;
                poolSize.descriptorCount = it->descriptorCount * maxSets;
                poolSizes.push_back(poolSize);
            }
        }

        // Build descriptorPool
        VkDescriptorPoolCreateInfo descrPoolInfo{};
        descrPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descrPoolInfo.maxSets = maxSets;
        descrPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descrPoolInfo.pPoolSizes = poolSizes.data();
        descrPoolInfo.flags = 0;

        if (vkCreateDescriptorPool(device, &descrPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }


        std::vector<VkDescriptorSetLayout> layouts(maxSets, descriptorSetLayout);
        // Allocate DescriptorSet
        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = maxSets;
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(maxSets);
        if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }
	}

	void DescriptorWrap::Destroy(VkDevice device)
	{
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	}

    // Write 1 buffer for multiple frames
    void DescriptorWrap::Write(VkDevice& device, uint32_t index, const VkBuffer& buffer, const uint32_t maxSets)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        for (size_t i = 0; i < maxSets; i++) // In Scanline Buffer maxSets is MAX_FRAME_IN_FLIGHT
        {
            VkWriteDescriptorSet descriptorWrite;
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.pNext = nullptr;
            descriptorWrite.dstSet = this->descriptorSets[i];
            descriptorWrite.dstBinding = index;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.descriptorType = bindingTable[index].descriptorType;
            descriptorWrite.pBufferInfo = &bufferInfo;

            assert(bindingTable[index].binding == index);

            assert(descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);

            vkUpdateDescriptorSets(device,
                                   1,
                                   &descriptorWrite,
                                   0,
                                   nullptr);
        }
    }

    // Write 1-to-1 buffers[] to frames
    void DescriptorWrap::Write(VkDevice& device, uint32_t index, const std::vector<BufferWrap>& bufferWraps)
    {
        for (size_t i = 0; i < bufferWraps.size(); i++) // In Scanline Buffer bufferWraps.size() is MAX_FRAME_IN_FLIGHT
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = bufferWraps[i].buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;

            VkWriteDescriptorSet descriptorWrite;
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.pNext = nullptr;
            descriptorWrite.dstSet = this->descriptorSets[i];
            descriptorWrite.dstBinding = index;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorCount = bindingTable[index].descriptorCount;
            descriptorWrite.descriptorType = bindingTable[index].descriptorType;
            descriptorWrite.pBufferInfo = &bufferInfo;

            assert(bindingTable[index].binding == index);

            assert(descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);

            vkUpdateDescriptorSets(device,
                                   1,
                                   &descriptorWrite,
                                   0,
                                   nullptr);
        }
    }

    void DescriptorWrap::Write(VkDevice& device, uint32_t index, const ImageWrap& texture, uint32_t maxSets)
    {
        VkDescriptorImageInfo textureImageInfo = texture.GetDescriptorImageInfo();

        // In Scanline Buffer maxSets is MAX_FRAME_IN_FLIGHT
        for (size_t i = 0; i < maxSets; i++)
        {
            VkWriteDescriptorSet descriptorWrite{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            descriptorWrite.dstSet = this->descriptorSets[i];
            descriptorWrite.dstBinding = index;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.descriptorType = bindingTable[index].descriptorType;
            descriptorWrite.pImageInfo = &textureImageInfo;

            assert(bindingTable[index].binding == index);

            assert(descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);

            vkUpdateDescriptorSets(device,
                1,
                &descriptorWrite,
                0,
                nullptr);
        }
    }

    void DescriptorWrap::Write(VkDevice& device, uint32_t index, const std::vector<ImageWrap>& textures, const uint32_t maxSets)
    {
        std::vector<VkDescriptorImageInfo> des;
        for (auto& texture : textures)
            des.emplace_back(texture.GetDescriptorImageInfo());

        // In Scanline Buffer maxSets is MAX_FRAME_IN_FLIGHT
        for (size_t i = 0; i < maxSets; i++) 
        {
            VkWriteDescriptorSet descriptorWrite{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            descriptorWrite.dstSet = this->descriptorSets[i];
            descriptorWrite.dstBinding = index;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorCount = des.size();
            descriptorWrite.descriptorType = bindingTable[index].descriptorType;
            descriptorWrite.pImageInfo = des.data();

            assert(bindingTable[index].binding == index);

            assert(descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);

            vkUpdateDescriptorSets(device,
                1,
                &descriptorWrite,
                0,
                nullptr);
        }
    }

    /// <summary>
    /// Write From FrameBuffers
    /// </summary>
    /// <param name="device"></param>
    /// <param name="index"></param>
    /// <param name="textures"></param>
    void DescriptorWrap::WriteFromFrameBuffers(VkDevice& device, uint32_t index, const std::vector<ImageWrap>& textures)
    {
        assert(textures.size() == MAX_FRAMES_IN_FLIGHT, "Frame Buffers target has size != MAX_FRAMES_IN_FLIGHT");

        std::vector<VkDescriptorImageInfo> des;
        for (auto& texture : textures)
            des.emplace_back(texture.GetDescriptorImageInfo());

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkWriteDescriptorSet descriptorWrite{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            descriptorWrite.dstSet = this->descriptorSets[i];
            descriptorWrite.dstBinding = index;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.descriptorType = bindingTable[index].descriptorType;
            descriptorWrite.pImageInfo = &des[i];

            assert(bindingTable[index].binding == index);

            assert(descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);

            vkUpdateDescriptorSets(device,
                1,
                &descriptorWrite,
                0,
                nullptr);
        }
    }

    /// <summary>
    /// Write from single Frame Buffer
    /// </summary>
    /// <param name="device"></param>
    /// <param name="index"></param>
    /// <param name="texture"></param>
    void DescriptorWrap::WriteFromFrameBuffer(VkDevice& device, uint32_t index, const ImageWrap& texture)
    {
        VkDescriptorImageInfo textureImageInfo = texture.GetDescriptorImageInfo();

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkWriteDescriptorSet descriptorWrite{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            descriptorWrite.dstSet = this->descriptorSets[i];
            descriptorWrite.dstBinding = index;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.descriptorType = bindingTable[index].descriptorType;
            descriptorWrite.pImageInfo = &textureImageInfo;

            assert(bindingTable[index].binding == index);

            assert(descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
                descriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);

            vkUpdateDescriptorSets(device,
                1,
                &descriptorWrite,
                0,
                nullptr);
        }
    }
}

