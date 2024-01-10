#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include <random>
#include <algorithm>

#include "../shaders/SharedStruct.h"
#include "ParticleSystemConstant.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::CreateComputeParticleBufferWraps()
    {
        // Initialize particles
        std::default_random_engine rndEngine((unsigned)time(nullptr));
        std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

        // Initial particle positions on a circle
        std::vector<Particle> particles(PARTICLE_COUNT);
        for (auto& particle : particles) 
        {
            float r = 0.25f * sqrt(rndDist(rndEngine));
            float theta = rndDist(rndEngine) * 2 * 3.14159265358979323846;
            float x = r * cos(theta) * appSwapchainExtent.height / appSwapchainExtent.width;
            float y = r * sin(theta);
            particle.position = glm::vec2(x, y);
            particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.00025f;
            particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
        }

        VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_COUNT;

        // Create a staging buffer used to upload data to the gpu
        BufferWrap stagingBufferWrap = CreateBufferWrap(bufferSize, 
                                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* data;
        vkMapMemory(appDevice, stagingBufferWrap.memory, 0, bufferSize, 0, &data);
        memcpy(data, particles.data(), (size_t)bufferSize);
        vkUnmapMemory(appDevice, stagingBufferWrap.memory);

        appComputeParticleStorageBufferWraps.resize(MAX_FRAMES_IN_FLIGHT);
        
        // Copy initial particle data to all storage buffers
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
            appComputeParticleStorageBufferWraps[i] = CreateBufferWrap(bufferSize,
                                                                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
                                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            CopyBuffer(stagingBufferWrap.buffer, appComputeParticleStorageBufferWraps[i].buffer, bufferSize);
        }

        stagingBufferWrap.Destroy(appDevice);
    }

    void ReinkanApp::CreateComputeParticleDescriptorSetWrap()
    {
        std::vector<VkDescriptorSetLayoutBinding> bindingTable;

        uint32_t bindingIndex = 0;
        // ComputeParticleUniformBufferObject
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                  bindingIndex++,                                               // binding;
                                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                            // descriptorType;
                                  1,                                                            // descriptorCount; 
                                  VK_SHADER_STAGE_COMPUTE_BIT });                               // stageFlags;
        // Last Frame Storage
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                  bindingIndex++,                                               // binding;
                                  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,                            // descriptorType;
                                  1,                                                            // descriptorCount; 
                                  VK_SHADER_STAGE_COMPUTE_BIT });                               // stageFlags;
        // Current Frame Storage
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                  bindingIndex++,                                               // binding;
                                  VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,                            // descriptorType;
                                  1,                                                            // descriptorCount; 
                                  VK_SHADER_STAGE_COMPUTE_BIT });                               // stageFlags;

        appComputeParticleDescriptorWrap.SetBindings(appDevice, bindingTable, MAX_FRAMES_IN_FLIGHT);

        // write UBO
        //appComputeParticleDescriptorWrap.Write();
        VkDeviceSize bufferSize = sizeof(ComputeParticleUniformBufferObject);

        appComputeParticleUBO.resize(MAX_FRAMES_IN_FLIGHT);
        appComputeParticleUBOMapped.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appComputeParticleUBO[i] = CreateBufferWrap(bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkMapMemory(appDevice, appComputeParticleUBO[i].memory, 0, bufferSize, 0, &appComputeParticleUBOMapped[i]);
        }
        appComputeParticleDescriptorWrap.Write(appDevice, 0, appComputeParticleUBO);

        // @@ Need to fix to sliding window instead of swap to support N MAX_FRAMES_IN_FLIGHT
        /* Sliding
        BufferWrap temp = appComputeParticleStorageBufferWraps[0];
        appComputeParticleStorageBufferWraps.erase(appComputeParticleStorageBufferWraps.begin());
        appComputeParticleStorageBufferWraps.push_back(temp);
        */
        std::swap(appComputeParticleStorageBufferWraps[0], appComputeParticleStorageBufferWraps[1]);
        // write i-1 frame
        appComputeParticleDescriptorWrap.Write(appDevice, 1, appComputeParticleStorageBufferWraps);
        // this is wrong 
        // we want binding 1              Without swap
        // Desc[0] = BW[0]        ||     Desc[0] = BW[0]      
        // Desc[1] = BW[1]        ||     Desc[1] = BW[1]

        std::swap(appComputeParticleStorageBufferWraps[0], appComputeParticleStorageBufferWraps[1]);
        // write i frame
        appComputeParticleDescriptorWrap.Write(appDevice, 2, appComputeParticleStorageBufferWraps);
        // we want binding 2
        // Desc[0] = BW[1]        ||     Desc[0] = BW[0]       
        // Desc[1] = BW[0]        ||     Desc[1] = BW[1]
    }

    void ReinkanApp::CreateComputeParticleSyncObjects()
    {
        appComputeParticleInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        appComputeParticleFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
        {
                if (vkCreateSemaphore(appDevice, &semaphoreInfo, nullptr, &appComputeParticleFinishedSemaphores[i]) != VK_SUCCESS 
                    || vkCreateFence(appDevice, &fenceInfo, nullptr, &appComputeParticleInFlightFences[i]) != VK_SUCCESS) 
                {
                    throw std::runtime_error("failed to create compute synchronization objects for a frame!");
                }
        }
    }

    void ReinkanApp::CreateComputeParticleCommandBuffer()
    {
        appComputeParticleCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = appCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)appComputeParticleCommandBuffers.size();

        if (vkAllocateCommandBuffers(appDevice, &allocInfo, appComputeParticleCommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate compute command buffers!");
        }
    }

    void ReinkanApp::UpdateComputeParticleUBO(uint32_t currentImage)
    {
        ComputeParticleUniformBufferObject ubo{};
        ubo.deltaTime = appLastFrameTime * 2.0f;

        memcpy(appComputeParticleUBOMapped[currentImage], &ubo, sizeof(ubo));
    }

    VkVertexInputBindingDescription ReinkanApp::GetParticleBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Particle);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    std::array<VkVertexInputAttributeDescription, 2> ReinkanApp::GetParticleAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Particle, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Particle, color);

        return attributeDescriptions;
    }
}