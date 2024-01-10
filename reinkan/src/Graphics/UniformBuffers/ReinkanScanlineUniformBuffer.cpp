#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

namespace Reinkan::Graphics
{
    void ReinkanApp::UpdateScanlineUBO(uint32_t currentImage)
    {
        UniformBufferObject ubo{};

        ubo.view = appMainCamera->GetViewMatrix();
        ubo.viewInverse = glm::inverse(ubo.view);
        ubo.proj = appMainCamera->GetPerspectiveMatrix();
        ubo.globalLightPosition = appGlobalLightPosition;

        ubo.shadowProjectionViewMatrix = appShadowProjectionViewMatrix;

        auto time = Core::TimeSystemLocator().GetTime();
        //ubo.model = glm::rotate(glm::mat4(1.0f), static_cast<float>(time->GetElapseTime() * glm::radians(90.0f)) * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
         
        ubo.screenExtent = glm::vec2(appSwapchainExtent.width, appSwapchainExtent.height);

        // CPU to update buffer req: VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        memcpy(appScanlineUBOMapped[currentImage], &ubo, sizeof(ubo));

        // GPU to update buffer on VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        //vkCmdUpdateBuffer(...)
        // as vkCmd...
        // Need command buffer and vkCmdPipelineBarrier to wait until buffer is ready to write
        // 
        // Barrier : VK_ACCESS_SHADER_READ_BIT -> VK_ACCESS_TRANSFER_WRITE_BIT
        // vkCmdUpdateBuffer(...)
        // Barrier : VK_ACCESS_TRANSFER_WRITE_BIT -> VK_ACCESS_SHADER_READ_BIT


        // Update wheel for volumitric light
        if (appObjects.size() > 0)
        {
            float angle = static_cast<float>(time->GetElapseTime() * glm::radians(90.0f)) * 0.5f;
            glm::mat4 transform = glm::mat4(1);
            transform = glm::scale(transform, glm::vec3(0.3));
            transform = glm::translate(transform, glm::vec3(18.0f, 18.0f, 1.5f));
            transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0, 0, 1));
            transform = glm::rotate(transform, angle, glm::vec3(0.0f, 1.0f, 0.0f));
            appObjects[0].transform = transform;

        }

    }
}

