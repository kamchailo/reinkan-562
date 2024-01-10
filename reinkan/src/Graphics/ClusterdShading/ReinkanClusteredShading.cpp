#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include "../shaders/SharedStruct.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::CreateComputeClusteredBufferWraps(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ, float nearClippingPlane, float farClippingPlane)
    {
        CreateComputeClusteredUBO();

        CreateComputeClusteredPlanes(nearClippingPlane, farClippingPlane, sizeZ);

        CreateComputeClusteredGrids(sizeX, sizeY, sizeZ);

        appClusteredSizeX = sizeX;
        appClusteredSizeY = sizeY;
        appClusteredSizeZ = sizeZ;

        if (appLightObjects.size() == 0) { return; }

        CreateComputeClusteredGlobalLights();

        CreateComputeClusteredLightBuffers(sizeX, sizeY, sizeZ);

        CreateComputeClusteredGlobalIndexCount();

    }

    void ReinkanApp::CreateComputeClusteredUBO()
    {
        //std::vector<BufferWrap>         appClusteredUBO;
        //std::vector<void*>              appClusteredUBOMapped;
        VkDeviceSize uboBufferSize = sizeof(ComputeClusteredUniformBufferObject);

        appClusteredUBO.resize(MAX_FRAMES_IN_FLIGHT);
        appClusteredUBOMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appClusteredUBO[i] = CreateBufferWrap(uboBufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkMapMemory(appDevice, appClusteredUBO[i].memory, 0, uboBufferSize, 0, &appClusteredUBOMapped[i]);
        }
    }

    void ReinkanApp::CreateComputeClusteredPlanes(float nearClippingPlane, float farClippingPlane, uint32_t sizeZ)
    {
        // appClusteredPlanes
        std::vector<ClusterPlane> clusterPlanes;
        clusterPlanes.resize(sizeZ);

        float currentZFar = nearClippingPlane;
        float currentZNear = nearClippingPlane;
        for (int i = 0; i < sizeZ; ++i)
        {
            // take near from last far
            currentZNear = currentZFar;
            clusterPlanes[i].zNear = currentZNear;

            currentZFar = nearClippingPlane * pow(farClippingPlane / nearClippingPlane, static_cast<float>(i+1) / sizeZ);
            clusterPlanes[i].zFar = currentZFar;

            //std::printf("plane %d: near %f far %f\n", i, clusterPlanes[i].zNear, clusterPlanes[i].zFar);
        }

        VkDeviceSize bufferSize = sizeof(ClusterPlane) * sizeZ;
        appClusteredPlanes = CreateStagedBufferWrap(bufferSize, clusterPlanes.data(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

    }

    void ReinkanApp::CreateComputeClusteredGrids(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ)
    {
        // out clusteredGrid       in clusteredCullLight
        //std::vector<BufferWrap>         appClusteredGrids;
        uint32_t bufferSize = sizeof(ClusterGrid) * sizeX * sizeY * sizeZ;

        appClusteredGrids = CreateBufferWrap(bufferSize,
                                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    void ReinkanApp::CreateComputeClusteredGlobalLights()
    {
        // readonly in clusteredCullLight  readonly in Scanline
        //BufferWrap                      appClusteredGlobalLights;
        appClusteredGlobalLights = CreateStagedBufferWrap(appLightObjects, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
        appClusteredGlobalLights_DEBUG = CreateStagedBufferWrap(appLightObjects, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
    }

    void ReinkanApp::CreateComputeClusteredLightBuffers(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ)
    {
        // out clusteredCullLight          readonly in Scanline
        //std::vector<BufferWrap>         appClusteredLightIndexMap;
        uint32_t maxBufferSize = 2 * sizeof(uint32_t) * appLightObjects.size() * sizeX * sizeY * sizeZ; // N lights * M Grids

        // out clusteredCullLight          readonly in Scanline
        //std::vector<BufferWrap>         appClusteredLightGrid;
        uint32_t bufferSize = 2 * sizeof(LightGrid) * sizeX * sizeY * sizeZ;

        appClusteredLightIndexMap.resize(MAX_FRAMES_IN_FLIGHT);
        appClusteredLightGrid.resize(MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appClusteredLightIndexMap[i] = CreateBufferWrap(maxBufferSize,
                                                            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            appClusteredLightGrid[i] = CreateBufferWrap(bufferSize,
                                                        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        }
    }

    void ReinkanApp::CreateComputeClusteredDescriptorSetWrap()
    {   
        //DescriptorWrap                  appClusteredGridDescriptorWrap;
        std::vector<VkDescriptorSetLayoutBinding> bindingTableClusteredGrid;

        uint32_t bindingTableClusteredGridIndex = 0;
        // ComputeClusteredUniformBufferObject
        bindingTableClusteredGrid.emplace_back(VkDescriptorSetLayoutBinding{
                                               bindingTableClusteredGridIndex++,    // binding;
                                               VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,   // descriptorType;
                                               1,                                   // descriptorCount; 
                                               VK_SHADER_STAGE_COMPUTE_BIT });      // stageFlags;

        // ClusterPlaneBlock
        bindingTableClusteredGrid.emplace_back(VkDescriptorSetLayoutBinding{
                                               bindingTableClusteredGridIndex++,    // binding;
                                               VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,   // descriptorType;
                                               1,                                   // descriptorCount; 
                                               VK_SHADER_STAGE_COMPUTE_BIT });      // stageFlags;

        // ClusterGridBlock
        bindingTableClusteredGrid.emplace_back(VkDescriptorSetLayoutBinding{
                                               bindingTableClusteredGridIndex++,    // binding;
                                               VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,   // descriptorType;
                                               1,                                   // descriptorCount; 
                                               VK_SHADER_STAGE_COMPUTE_BIT });      // stageFlags;
        
        appClusteredGridDescriptorWrap.SetBindings(appDevice, bindingTableClusteredGrid, MAX_FRAMES_IN_FLIGHT);

        appClusteredGridDescriptorWrap.Write(appDevice, 0, appClusteredUBO);

        appClusteredGridDescriptorWrap.Write(appDevice, 1, appClusteredPlanes.buffer, MAX_FRAMES_IN_FLIGHT);

        appClusteredGridDescriptorWrap.Write(appDevice, 2, appClusteredGrids.buffer, MAX_FRAMES_IN_FLIGHT);

        //DescriptorWrap                  appClusteredCullLightDescriptorWrap;
        std::vector<VkDescriptorSetLayoutBinding> bindingTableCullLight;

        uint32_t bindingTableCullLightIndex = 0;
        // ComputeClusteredUniformBufferObject
        bindingTableCullLight.emplace_back(VkDescriptorSetLayoutBinding{
                                           bindingTableCullLightIndex++,        // binding;
                                           VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,   // descriptorType;
                                           1,                                   // descriptorCount; 
                                           VK_SHADER_STAGE_COMPUTE_BIT });      // stageFlags;

        // ClusterGridBlock
        bindingTableCullLight.emplace_back(VkDescriptorSetLayoutBinding{
                                               bindingTableCullLightIndex++,    // binding;
                                               VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,   // descriptorType;
                                               1,                                   // descriptorCount; 
                                               VK_SHADER_STAGE_COMPUTE_BIT });      // stageFlags;

        // GlobalLightSSBO
        bindingTableCullLight.emplace_back(VkDescriptorSetLayoutBinding{
                                           bindingTableCullLightIndex++,        // binding;
                                           VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,   // descriptorType;
                                           1,                                   // descriptorCount; 
                                           VK_SHADER_STAGE_COMPUTE_BIT });      // stageFlags;

        // LightIndexSSBO
        bindingTableCullLight.emplace_back(VkDescriptorSetLayoutBinding{
                                           bindingTableCullLightIndex++,        // binding;
                                           VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,   // descriptorType;
                                           1,                                   // descriptorCount; 
                                           VK_SHADER_STAGE_COMPUTE_BIT });      // stageFlags;

        // LightGridSSBO
        bindingTableCullLight.emplace_back(VkDescriptorSetLayoutBinding{
                                           bindingTableCullLightIndex++,        // binding;
                                           VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,   // descriptorType;
                                           1,                                   // descriptorCount; 
                                           VK_SHADER_STAGE_COMPUTE_BIT });      // stageFlags;

        // GlobalIndexCountSSBO
        bindingTableCullLight.emplace_back(VkDescriptorSetLayoutBinding{
                                           bindingTableCullLightIndex++,        // binding;
                                           VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,   // descriptorType;
                                           1,                                   // descriptorCount; 
                                           VK_SHADER_STAGE_COMPUTE_BIT });      // stageFlags;

        // GlobalLight_DEBUG_SSBO
        bindingTableCullLight.emplace_back(VkDescriptorSetLayoutBinding{
                                           bindingTableCullLightIndex++,        // binding;
                                           VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,   // descriptorType;
                                           1,                                   // descriptorCount; 
                                           VK_SHADER_STAGE_COMPUTE_BIT });      // stageFlags;

        appClusteredCullLightDescriptorWrap.SetBindings(appDevice, bindingTableCullLight, MAX_FRAMES_IN_FLIGHT);

        appClusteredCullLightDescriptorWrap.Write(appDevice, 0, appClusteredUBO);

        appClusteredCullLightDescriptorWrap.Write(appDevice, 1, appClusteredGrids.buffer, MAX_FRAMES_IN_FLIGHT);

        appClusteredCullLightDescriptorWrap.Write(appDevice, 2, appClusteredGlobalLights.buffer, MAX_FRAMES_IN_FLIGHT);

        appClusteredCullLightDescriptorWrap.Write(appDevice, 3, appClusteredLightIndexMap);

        appClusteredCullLightDescriptorWrap.Write(appDevice, 4, appClusteredLightGrid);

        appClusteredCullLightDescriptorWrap.Write(appDevice, 5, appClusteredGlobalIndexCount);

        appClusteredCullLightDescriptorWrap.Write(appDevice, 6, appClusteredGlobalLights_DEBUG.buffer, MAX_FRAMES_IN_FLIGHT);
    }

    void ReinkanApp::CreateComputeClusteredSyncObjects()
    {
        appComputeClusteredInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        appComputeClusteredFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(appDevice, &semaphoreInfo, nullptr, &appComputeClusteredFinishedSemaphores[i]) != VK_SUCCESS
                || vkCreateFence(appDevice, &fenceInfo, nullptr, &appComputeClusteredInFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create compute synchronization objects for a frame!");
            }
        }
    }

    void ReinkanApp::CreateComputeClusteredCommandBuffer()
    {
        appComputeClusteredCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(appPhysicalDevice);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

        if (queueFamilyIndices.computeOnlyFamily.has_value())
        {
            allocInfo.commandPool = appComputeCommandPool;
        }
        else
        {
            allocInfo.commandPool = appCommandPool;
        }

        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)appComputeClusteredCommandBuffers.size();

        if (vkAllocateCommandBuffers(appDevice, &allocInfo, appComputeClusteredCommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate compute command buffers!");
        }
    }

    void ReinkanApp::CreateComputeClusteredGlobalIndexCount()
    {
        appClusteredGlobalIndexCount.resize(MAX_FRAMES_IN_FLIGHT);

        uint32_t bufferSize = sizeof(uint32_t);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appClusteredGlobalIndexCount[i] = CreateBufferWrap(bufferSize,
                                                               VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        }
    }

    void ReinkanApp::UpdateComputeClusteredUBO(uint32_t currentImage)
    {
        ComputeClusteredUniformBufferObject ubo{};
        // update project and view
        ubo.view = appMainCamera->GetViewMatrix();;
        ubo.proj = appMainCamera->GetPerspectiveMatrix();
        ubo.projInverse = glm::inverse(ubo.proj);
        ubo.screenDimensions = glm::vec2(appSwapchainExtent.width, appSwapchainExtent.height);

        memcpy(appClusteredUBOMapped[currentImage], &ubo, sizeof(ubo));
    }

    void ReinkanApp::UpdateClusteredGrids()
    {
        auto commandBuffer = BeginTempCommandBuffer();

        UpdateComputeClusteredUBO(appCurrentFrame);

        {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, appClusteredGridPipeline);

            
            vkCmdBindDescriptorSets(commandBuffer,
                                    VK_PIPELINE_BIND_POINT_COMPUTE,
                                    appClusteredGridPipelineLayout,
                                    0,
                                    1,
                                    &appClusteredGridDescriptorWrap.descriptorSets[0],
                                    0,
                                    nullptr);

            vkCmdDispatch(commandBuffer, appClusteredSizeX, appClusteredSizeY, appClusteredSizeZ);

            VkMemoryBarrier  clusteredGridBarrier = {};
            clusteredGridBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
            clusteredGridBarrier.pNext = nullptr;
            clusteredGridBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            clusteredGridBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_DEPENDENCY_DEVICE_GROUP_BIT,
                1,
                &clusteredGridBarrier,
                0,
                nullptr,
                0,
                nullptr);
            
        }

        EndTempCommandBuffer(commandBuffer);
    }

}