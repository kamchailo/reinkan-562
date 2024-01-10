#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include <glm/gtx/transform.hpp>
#include "../shaders/SharedStruct.h"

namespace Reinkan::Graphics
{
    VkVertexInputBindingDescription ReinkanApp::GetDebugBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VLightVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    std::array<VkVertexInputAttributeDescription, 2> ReinkanApp::GetDebugAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VLightVertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VLightVertex, vertexNormal);

        return attributeDescriptions;
    }
    
    void ReinkanApp::CreateDebugBufferWraps()
    {
        // UBO [MAX_FRAMES_IN_FLIGHT]
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        appDebugUBO.resize(MAX_FRAMES_IN_FLIGHT);
        appDebugUBOMapped.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appDebugUBO[i] = CreateBufferWrap(bufferSize,
                                              VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkMapMemory(appDevice, appDebugUBO[i].memory, 0, bufferSize, 0, &appDebugUBOMapped[i]);
        }

        /*
        bufferSize = BUFFER_SIZE;
        appDebugStorageBufferWraps.resize(MAX_FRAMES_IN_FLIGHT);
        appDebugStorageMapped.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appDebugStorageBufferWraps[i] = CreateBufferWrap(bufferSize,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            vkMapMemory(appDevice, appDebugStorageBufferWraps[i].memory, 0, bufferSize, 0, &appDebugStorageMapped[i]);
        }
        */

    }
    

    void ReinkanApp::CreateDebugDescriptorSetWrap()
    {
        std::vector<VkDescriptorSetLayoutBinding> bindingTable;

        uint32_t bindingIndex = 0;
        // DebugUniformBufferObject
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                  bindingIndex++,                                                   // binding;
                                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                                // descriptorType;
                                  1,                                                                // descriptorCount; 
                                  VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT });     // stageFlags;

        appDebugDescriptorWrap.SetBindings(appDevice,
                                                bindingTable,
                                                MAX_FRAMES_IN_FLIGHT);

        appDebugDescriptorWrap.Write(appDevice, 0, appDebugUBO);
    }

    void ReinkanApp::UpdateDebugUBO(uint32_t currentImage)
    {
        UniformBufferObject ubo{};

        ubo.view = appMainCamera->GetViewMatrix();
        //glm::vec3 position = glm::vec3(0.0, 2.0, 0.0);
        //glm::vec3 cameraDirection = glm::normalize(glm::vec3(0.0, 0.0, 1.0));
        //ubo.view = glm::lookAt(position, position + cameraDirection, glm::vec3(0.0, 1.0, 0.0));
        ubo.view = appMainCamera->GetViewMatrix();
        ubo.viewInverse = glm::inverse(ubo.view);
        ubo.proj = appMainCamera->GetPerspectiveMatrix();

        ubo.model = glm::mat4(1);
        //ubo.model = glm::translate(glm::mat4(1), glm::vec3(0.0, 2.0, 0.0));
        //ubo.model = glm::rotate(glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0)) * ubo.model;

        ubo.screenExtent = glm::vec2(appSwapchainExtent.width, appSwapchainExtent.height);  

        // CPU to update buffer req: VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        memcpy(appDebugUBOMapped[currentImage], &ubo, sizeof(ubo));

    }
}