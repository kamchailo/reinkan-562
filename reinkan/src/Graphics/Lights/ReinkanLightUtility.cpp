#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::AppendLight(const LightObject& lightObject)
    {
        appLightObjects.push_back(lightObject);
    }

    void ReinkanApp::AppendLightMesh(const ModelData& modelData)
    {
        if (appLightMeshTypes.find(modelData.name) != appLightMeshTypes.end())
        {
            return;
        }

        ObjectData objData;
        objData.nbIndices = modelData.indices.size();
        objData.nbVertices = modelData.vertices.size();
        objData.vertexBufferWrap = CreateStagedBufferWrap(modelData.vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        objData.indexBufferWrap = CreateStagedBufferWrap(modelData.indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);;
        objData.name = modelData.name;
        objData.objectId = appLightMeshTypes.size();
        objData.materialId = -1;
        objData.transform = glm::mat4(1);

        appLightMeshTypes[modelData.name] = objData;
    }
}