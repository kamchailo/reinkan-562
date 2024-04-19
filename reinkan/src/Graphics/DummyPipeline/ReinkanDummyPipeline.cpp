#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::CreateDummyComputePipeline(uint32_t dummyWidth, uint32_t dummyHeight)
    {
        ///-----------------------
        ///      Resources
        ///-----------------------
        
        // Dummy Image
        appDummyImageWraps.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appDummyImageWraps[i] = CreateImageWrap(dummyWidth,
                dummyHeight,
                VK_FORMAT_R32G32B32A32_SFLOAT,                                           // Image Format
                VK_IMAGE_TILING_OPTIMAL,                                        // Image Tilling
                VK_IMAGE_USAGE_TRANSFER_DST_BIT
                | VK_IMAGE_USAGE_SAMPLED_BIT
                | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                | VK_IMAGE_USAGE_STORAGE_BIT
                | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,                                   // Image Usage
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,                            // Memory Property
                1,
                appMsaaSamples);

            TransitionImageLayout(appDummyImageWraps[i].image,
                VK_FORMAT_R32G32B32A32_SFLOAT,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_GENERAL);

            appDummyImageWraps[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            appDummyImageWraps[i].imageView = CreateImageView(appDummyImageWraps[i].image, VK_FORMAT_R32G32B32A32_SFLOAT);
            appDummyImageWraps[i].sampler = CreateNearestImageSampler();
        }

        // Dummy Buffer
        appDummyData.resize(dummyWidth * dummyHeight);
#pragma omp parallel for
        for (int i = 0; i < appDummyData.size(); ++i)
        {
            appDummyData[i].value = i;
        }

        VkDeviceSize bufferSize = sizeof(DummyData) * appDummyData.size();
        appDummyBuffer.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appDummyBuffer[i] = CreateBufferWrap(bufferSize,
                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        }

        ///-----------------------
        ///     Descriptor Set
        ///-----------------------
        std::vector<VkDescriptorSetLayoutBinding> bindingTable;
        uint32_t bindingIndex = 0;
        // UBO
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                  bindingIndex++,                                                   // binding;
                                  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,                                // descriptorType;
                                  1,                                                                // descriptorCount; 
                                  VK_SHADER_STAGE_COMPUTE_BIT });     // stageFlags;

        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                  bindingIndex++,                                                   // binding;
                                  VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,                                // descriptorType;
                                  2,                                                                // descriptorCount; 
                                  VK_SHADER_STAGE_COMPUTE_BIT });     // stageFlags;

        appDummyDescriptorWrap.SetBindings(appDevice, bindingTable, MAX_FRAMES_IN_FLIGHT);

        bindingIndex = 0;
        appDummyDescriptorWrap.Write(appDevice, bindingIndex++, appDummyBuffer);
        appDummyDescriptorWrap.Write(appDevice, bindingIndex++, appDummyImageWraps, MAX_FRAMES_IN_FLIGHT);

        ///-----------------------
        ///     Pipeline
        ///-----------------------
        {
            auto computeShaderCode = ReadFile("../shaders/dummyComputeShader.comp.spv");

            VkShaderModule computeShaderModule = CreateShaderModule(computeShaderCode);

            VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
            computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            computeShaderStageInfo.module = computeShaderModule;
            computeShaderStageInfo.pName = "main";

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = &appDummyDescriptorWrap.descriptorSetLayout;

            if (vkCreatePipelineLayout(appDevice, &pipelineLayoutInfo, nullptr, &appDummyPipelineLayout) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create compute pipeline layout!");
            }

            VkComputePipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
            pipelineInfo.layout = appDummyPipelineLayout;
            pipelineInfo.stage = computeShaderStageInfo;

            if (vkCreateComputePipelines(appDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &appDummyPipeline) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create compute pipeline!");
            }

            vkDestroyShaderModule(appDevice, computeShaderModule, nullptr);
        }
    }

    void ReinkanApp::RecordDummyCompute()
    {
    }

    void ReinkanApp::DestroyDummyResources()
    {
        appDummyDescriptorWrap.Destroy(appDevice);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appDummyImageWraps[i].Destroy(appDevice);
            appDummyBuffer[i].Destroy(appDevice);
        }

        vkDestroyPipeline(appDevice, appDummyPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appDummyPipelineLayout, nullptr);
    }
}