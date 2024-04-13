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

}