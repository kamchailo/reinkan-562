#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include <vector>

namespace Reinkan::Graphics
{
    void ReinkanApp::BindModelData()
    {
        for (auto modelData : appModelDataToBeLoaded)
        {
            ObjectData object;
            object.nbVertices = modelData.modelDataPtr->vertices.size();
            object.nbIndices = modelData.modelDataPtr->indices.size();
            object.vertexBufferWrap = CreateStagedBufferWrap(modelData.modelDataPtr->vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            object.indexBufferWrap = CreateStagedBufferWrap(modelData.modelDataPtr->indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

            object.name = modelData.modelDataPtr->name;

            object.objectId = appObjects.size();

            // only support one material per object
            object.materialId = modelData.modelDataPtr->materialIndex;

            // Object Instance Properties
            object.transform = modelData.modelTransform;

            appObjects.push_back(object);
        }

        appModelDataToBeLoaded.clear();
        
        VkPhysicalDeviceProperties gpuProperty;
        vkGetPhysicalDeviceProperties(appPhysicalDevice, &gpuProperty);
        std::printf("Max Descriptor Set Sampled Images: %d\n", gpuProperty.limits.maxDescriptorSetSampledImages);

    }

    void ReinkanApp::BindMaterials()
    {
        if (appMaterials.size() > 0)
        {
            appMaterialBufferWrap = CreateStagedBufferWrap(appMaterials, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
        }
    }

    void ReinkanApp::BindTextures()
    {
        // Need to change to use same memory with mem offset
        for (auto texturePath : appTexturePaths)
        {
            auto textureImageWrap = CreateTextureImageWrap(texturePath);
            TransitionImageLayout(textureImageWrap.image, 
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            appTextureImageWraps.push_back(textureImageWrap);
        }
    }

    void ReinkanApp::CreateScanlineUBO()
    {
        // UBO [MAX_FRAMES_IN_FLIGHT]
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        appScanlineUBO.resize(MAX_FRAMES_IN_FLIGHT);
        appScanlineUBOMapped.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appScanlineUBO[i] = CreateBufferWrap(bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkMapMemory(appDevice, appScanlineUBO[i].memory, 0, bufferSize, 0, &appScanlineUBOMapped[i]);
        }
    }

    void ReinkanApp::CreateScanlineDescriptorWrap()
    {

        //typedef struct VkDescriptorSetLayoutBinding {
        //    uint32_t              binding;
        //    VkDescriptorType      descriptorType;
        //    uint32_t              descriptorCount;
        //    VkShaderStageFlags    stageFlags;
        //    const VkSampler* pImmutableSamplers;
        //} VkDescriptorSetLayoutBinding;

        // layout(binding = 0) uniform UniformBufferObject_T
        std::vector<VkDescriptorSetLayoutBinding> bindingTable;
        uint32_t bindingIndex = 0;
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                  bindingIndex++,                                                   // binding;
                                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                                // descriptorType;
                                  1,                                                                // descriptorCount; 
                                  VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT });     // stageFlags;

        // layout(binding = 1) buffer MaterialBlock 
        if (appMaterials.size() > 0)
        {
            bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                                               // binding;
                                      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,                            // descriptorType;
                                      1,                                                            // descriptorCount;
                                      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT }); // stageFlags;
        }

        // layout(binding = 2) uniform sampler2D[] textureSamplers
        if (appTextureImageWraps.size() > 0)
        {
            bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                                               // binding;
                                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,                    // descriptorType;
                                      static_cast<uint32_t>(appTextureImageWraps.size()),           // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_FRAGMENT_BIT });                              // stageFlags;
        }

        // layout(std140, binding = 3) readonly buffer GlobalLightSSBO
        if (appLightObjects.size() > 0)
        {
            bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                                       // binding;
                                      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,                    // descriptorType;
                                      1,                                                    // descriptorCount; 
                                      VK_SHADER_STAGE_FRAGMENT_BIT });                      // stageFlags;
        }
        
        // layout(binding = 4) uniform sampler2D shadowmap;
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                                               // binding;
                                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,                    // descriptorType;
                                      static_cast<uint32_t>(appShadowMapImageWraps.size()),           // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_FRAGMENT_BIT });


        appScanlineDescriptorWrap.SetBindings(appDevice,
                                              bindingTable, 
                                              MAX_FRAMES_IN_FLIGHT);

        
        bindingIndex = 0;

        // 0 - UBO
        appScanlineDescriptorWrap.Write(appDevice, bindingIndex++, appScanlineUBO);
        
        // 1 - Material only once
        if (appMaterials.size() > 0)
        {
            appScanlineDescriptorWrap.Write(appDevice, bindingIndex++, appMaterialBufferWrap.buffer, MAX_FRAMES_IN_FLIGHT);
        }

        // 2 - Texture only once
        if (appTextureImageWraps.size() > 0)
        {
            appScanlineDescriptorWrap.Write(appDevice, bindingIndex++, appTextureImageWraps, MAX_FRAMES_IN_FLIGHT);
        }

        // 3 - Light Objects
        if (appLightObjects.size() > 0)
        {
            appScanlineDescriptorWrap.Write(appDevice, bindingIndex++, appClusteredGlobalLights.buffer, MAX_FRAMES_IN_FLIGHT);
        }

        // 4 - Shadow Map
        appScanlineDescriptorWrap.Write(appDevice, bindingIndex++, appShadowMapImageWraps, MAX_FRAMES_IN_FLIGHT);

    }
}