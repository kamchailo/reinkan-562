#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include "glm/gtx/transform.hpp"

namespace Reinkan::Graphics
{
	void ReinkanApp::CreateShadowRenderPass()
	{
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = FindDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(appDevice, &renderPassInfo, nullptr, &appShadowRenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
	}

    void ReinkanApp::CreateShadowDescriptorSetWrap()
    {

        std::vector<VkDescriptorSetLayoutBinding> bindingTable;
        uint32_t bindingIndex = 0;
        // UBO
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                  bindingIndex++,                                                   // binding;
                                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                                // descriptorType;
                                  1,                                                                // descriptorCount; 
                                  VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT });     // stageFlags;

        appShadowDescriptorWrap.SetBindings(appDevice, bindingTable, MAX_FRAMES_IN_FLIGHT);

        appShadowDescriptorWrap.Write(appDevice, 0, appShadowUBO);
    }

	void ReinkanApp::CreateShadowPipeline(DescriptorWrap descriptorWrap)
	{
        auto vertShaderCode = ReadFile("../shaders/shadow.vert.spv");
        auto fragShaderCode = ReadFile("../shaders/shadow.frag.spv");

        VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        // Need dedicate Binding Description for Shadow map
        auto bindingDescription = GetBindingDescription();
        auto attributeDescriptions = GetAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(appShadowMapWidth);
        viewport.height = static_cast<float>(appShadowMapHeight);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent.width = appShadowMapWidth;
        scissor.extent.height = appShadowMapHeight;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.minSampleShading = 0.2f; // min fraction for sample shading; closer to one is smoother
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;// BEWARE!!  NECESSARY!!
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 0;
        dynamicState.pDynamicStates = nullptr;

        VkPushConstantRange pushConstantRanges = {};
        pushConstantRanges.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRanges.offset = 0;
        pushConstantRanges.size = sizeof(PushConstantShadow);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorWrap.descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRanges;

        if (vkCreatePipelineLayout(appDevice, &pipelineLayoutInfo, nullptr, &appShadowPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = appShadowPipelineLayout;
        pipelineInfo.renderPass = appShadowRenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(appDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &appShadowPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(appDevice, fragShaderModule, nullptr);
        vkDestroyShaderModule(appDevice, vertShaderModule, nullptr);
	}

    void ReinkanApp::CreateShadowResources(size_t width, size_t height)
    {
        appShadowMapWidth = width;
        appShadowMapHeight = height;

        // UBO [MAX_FRAMES_IN_FLIGHT]
        VkDeviceSize bufferSize = sizeof(ShadowUniformBufferObject);
        appShadowUBO.resize(MAX_FRAMES_IN_FLIGHT);
        appShadowUBOMapped.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appShadowUBO[i] = CreateBufferWrap(bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkMapMemory(appDevice, appShadowUBO[i].memory, 0, bufferSize, 0, &appShadowUBOMapped[i]);
        }
    }

    void ReinkanApp::CreateShadowBlurDescriptorSetWrap()
    {
        std::vector<VkDescriptorSetLayoutBinding> bindingTable;
        uint32_t bindingIndex = 0;
        // UBO
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                  bindingIndex++,                                                   // binding;
                                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                                // descriptorType;
                                  1,                                                                // descriptorCount; 
                                  VK_SHADER_STAGE_COMPUTE_BIT });     // stageFlags;

        // ShadowImageWrap
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                                               // binding;
                                      VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,                    // descriptorType;
                                      2,                                         // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_COMPUTE_BIT });                              // stageFlags;

        // appBlurShadowMapImageWraps
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                                               // binding;
                                      VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,                    // descriptorType;
                                      2,                                         // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_COMPUTE_BIT });

        /*
        * 
        * Binding Resources
        * - Gaussian Blur Array
        * - Source Images
        * - Destination Images
        *   
        */

        appShadowBlurDescriptorWrap.SetBindings(appDevice, bindingTable, MAX_FRAMES_IN_FLIGHT);

        bindingIndex = 0;
        appShadowBlurDescriptorWrap.Write(appDevice, bindingIndex++, appShadowBlurUBO);
        appShadowBlurDescriptorWrap.Write(appDevice, bindingIndex++, appShadowMapImageWraps, MAX_FRAMES_IN_FLIGHT);
        appShadowBlurDescriptorWrap.Write(appDevice, bindingIndex++, appBlurShadowMapImageWraps, MAX_FRAMES_IN_FLIGHT);
        
    }

    void ReinkanApp::CreateShadowBlurHorizontalPipeline(DescriptorWrap descriptorWrap)
    {
        auto computeShaderCode = ReadFile("../shaders/shadowBlurHorizontal.comp.spv");

        VkShaderModule computeShaderModule = CreateShaderModule(computeShaderCode);

        VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
        computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computeShaderStageInfo.module = computeShaderModule;
        computeShaderStageInfo.pName = "main";

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorWrap.descriptorSetLayout;

        if (vkCreatePipelineLayout(appDevice, &pipelineLayoutInfo, nullptr, &appShadowBlurHorizontalPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute pipeline layout!");
        }

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = appShadowBlurHorizontalPipelineLayout;
        pipelineInfo.stage = computeShaderStageInfo;

        if (vkCreateComputePipelines(appDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &appShadowBlurHorizontalPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute pipeline!");
        }

        vkDestroyShaderModule(appDevice, computeShaderModule, nullptr);
    }

    void ReinkanApp::CreateShadowBlurVerticalPipeline(DescriptorWrap descriptorWrap)
    {
        auto computeShaderCode = ReadFile("../shaders/shadowBlurVertical.comp.spv");

        VkShaderModule computeShaderModule = CreateShaderModule(computeShaderCode);

        VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
        computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computeShaderStageInfo.module = computeShaderModule;
        computeShaderStageInfo.pName = "main";

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorWrap.descriptorSetLayout;

        if (vkCreatePipelineLayout(appDevice, &pipelineLayoutInfo, nullptr, &appShadowBlurVerticalPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute pipeline layout!");
        }

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = appShadowBlurVerticalPipelineLayout;
        pipelineInfo.stage = computeShaderStageInfo;

        if (vkCreateComputePipelines(appDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &appShadowBlurVerticalPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute pipeline!");
        }

        vkDestroyShaderModule(appDevice, computeShaderModule, nullptr);
    }

    void ReinkanApp::CreateShadowBlurResources()
    {
        // UBO [MAX_FRAMES_IN_FLIGHT]
        VkDeviceSize bufferSize = sizeof(ShadowBlurUniformBufferObject);
        appShadowBlurUBO.resize(MAX_FRAMES_IN_FLIGHT);
        appShadowBlurUBOMapped.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appShadowBlurUBO[i] = CreateBufferWrap(bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkMapMemory(appDevice, appShadowBlurUBO[i].memory, 0, bufferSize, 0, &appShadowBlurUBOMapped[i]);
        }

        // Blurred Shadow Map
        appBlurShadowMapImageWraps.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appBlurShadowMapImageWraps[i] = CreateImageWrap(appShadowMapWidth,
                appShadowMapHeight,
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

            TransitionImageLayout(appBlurShadowMapImageWraps[i].image,
                VK_FORMAT_R32G32B32A32_SFLOAT,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_GENERAL);

            appBlurShadowMapImageWraps[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

            appBlurShadowMapImageWraps[i].imageView = CreateImageView(appBlurShadowMapImageWraps[i].image, VK_FORMAT_R32G32B32A32_SFLOAT);
            
            appBlurShadowMapImageWraps[i].sampler = CreateNearestImageSampler();
        }

    }

    void ReinkanApp::CreateShadowCommandBuffer()
    {
        appShadowCommandBuffer.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = appCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)appShadowCommandBuffer.size();

        if (vkAllocateCommandBuffers(appDevice, &allocInfo, appShadowCommandBuffer.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate compute command buffers!");
        }
    }

    void ReinkanApp::CreateShadowSyncObjects()
    {
        appPreComputeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        appComputeShadowBlurFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        appRenderShadowFences.resize(MAX_FRAMES_IN_FLIGHT);
        appComputeShadowBlurFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(appDevice, &semaphoreInfo, nullptr, &appPreComputeFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(appDevice, &semaphoreInfo, nullptr, &appComputeShadowBlurFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(appDevice, &fenceInfo, nullptr, &appRenderShadowFences[i]) != VK_SUCCESS ||
                vkCreateFence(appDevice, &fenceInfo, nullptr, &appComputeShadowBlurFences[i]) != VK_SUCCESS)
            {

                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void ReinkanApp::UpdateShadowUBO(uint32_t currentImage)
    {
        ShadowUniformBufferObject ubo{};

        ubo.view = glm::lookAt(appGlobalLightPosition, appGlobalLightPosition + appGlobalLightDirection, glm::vec3(0.0, 1.0, 0.0));
        ubo.viewInverse = glm::inverse(ubo.view);

        float fovy = appDebugFloat3;

        glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(fovy), static_cast<float>(appShadowMapWidth) / appShadowMapHeight, 0.1f, 1000.0f);
        perspectiveMatrix[1][1] *= -1;

        ubo.proj = perspectiveMatrix;

        glm::mat4 offset = glm::translate(glm::mat4(1), glm::vec3(0.5)) * glm::scale(glm::mat4(1), glm::vec3(0.5));

        appShadowProjectionViewMatrix = offset * ubo.proj * ubo.view;

        //auto time = Core::TimeSystemLocator().GetTime();
        //ubo.model = glm::rotate(glm::mat4(1.0f), static_cast<float>(time->GetElapseTime() * glm::radians(90.0f)) * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

        ubo.screenExtent = glm::vec2(appShadowMapWidth, appShadowMapHeight);

        // CPU to update buffer req: VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        memcpy(appShadowUBOMapped[currentImage], &ubo, sizeof(ubo));
    }

    void ReinkanApp::UpdateShadowBlurUBO(uint32_t currentImage)
    {
        ShadowBlurUniformBufferObject ubo{};

        ubo.blurWidth = 100;
        ubo.shadowWidth = appShadowMapWidth;
        ubo.shadowHeight = appShadowMapHeight;

        // CPU to update buffer req: VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        memcpy(appShadowBlurUBOMapped[currentImage], &ubo, sizeof(ubo));
    }

}