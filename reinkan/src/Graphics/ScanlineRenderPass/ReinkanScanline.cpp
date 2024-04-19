#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::CreateScanlineRenderPass()
    {
        // Albedo
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = appSwapchainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

        VkAttachmentDescription positionAttachment{};
        positionAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        positionAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        positionAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        positionAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        positionAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        positionAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        positionAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        positionAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

        VkAttachmentDescription normalAttachment{};
        normalAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        normalAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        normalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        normalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        normalAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        normalAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        normalAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        normalAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

        VkAttachmentDescription specularAttachment{};
        specularAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        specularAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        specularAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        specularAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        specularAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        specularAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        specularAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        specularAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = FindDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference positionAttachmentRef{};
        positionAttachmentRef.attachment = 1;
        positionAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference normalAttachmentRef{};
        normalAttachmentRef.attachment = 2;
        normalAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference specularAttachmentRef{};
        specularAttachmentRef.attachment = 3;
        specularAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 4;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        //VkAttachmentReference colorAttachmentResolveRef{};
        //colorAttachmentResolveRef.attachment = 2;
        //colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        std::array< VkAttachmentReference, 4> colorAttachmentRefs;
        colorAttachmentRefs[0] = colorAttachmentRef;
        colorAttachmentRefs[1] = positionAttachmentRef;
        colorAttachmentRefs[2] = normalAttachmentRef;
        colorAttachmentRefs[3] = specularAttachmentRef;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());
        subpass.pColorAttachments = colorAttachmentRefs.data();
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        //subpass.pResolveAttachments = &colorAttachmentResolveRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 5> attachments = { colorAttachment, 
                                                                positionAttachment, 
                                                                normalAttachment,
                                                                specularAttachment,
                                                                depthAttachment };

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(appDevice, &renderPassInfo, nullptr, &appScanlineRenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
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
        appScanlineDescriptorWrap.Write(appDevice, bindingIndex++, appBlurShadowMapImageWraps, MAX_FRAMES_IN_FLIGHT);

    }
}
