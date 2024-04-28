#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{

	void ReinkanApp::CreateAORenderPass()
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

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 1> attachments = { colorAttachment };

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(appDevice, &renderPassInfo, nullptr, &appAORenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
	}


    void ReinkanApp::CreateAODescriptorSetWrap()
    {
        std::vector<VkDescriptorSetLayoutBinding> bindingTable;
        uint32_t bindingIndex = 0;

        // 0 - ScanlineImageWrap
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                                               // binding;
                                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,                    // descriptorType;
                                      MAX_FRAMES_IN_FLIGHT,                                         // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_FRAGMENT_BIT });                              // stageFlags;

        // 1 - ScanlinePositionImageWrap
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                                               // binding;
                                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,                    // descriptorType;
                                      MAX_FRAMES_IN_FLIGHT,                                         // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_FRAGMENT_BIT });                              // stageFlags;

        // 2 - ScanlineNormalImageWrap
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                                               // binding;
                                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,                    // descriptorType;
                                      MAX_FRAMES_IN_FLIGHT,                                         // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_FRAGMENT_BIT });                              // stageFlags;

        // 3 - ScanlineSpecularImageWrap
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                                               // binding;
                                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,                    // descriptorType;
                                      MAX_FRAMES_IN_FLIGHT,                                         // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_FRAGMENT_BIT });                              // stageFlags;

        appAODescriptorWrap.SetBindings(appDevice, bindingTable, MAX_FRAMES_IN_FLIGHT);

        bindingIndex = 0;
        appAODescriptorWrap.Write(appDevice, bindingIndex++, appScanlineImageWraps, MAX_FRAMES_IN_FLIGHT);
        appAODescriptorWrap.Write(appDevice, bindingIndex++, appScanlinePositionImageWraps, MAX_FRAMES_IN_FLIGHT);
        appAODescriptorWrap.Write(appDevice, bindingIndex++, appScanlineNormalImageWraps, MAX_FRAMES_IN_FLIGHT);
        appAODescriptorWrap.Write(appDevice, bindingIndex++, appScanlineSpecularImageWraps, MAX_FRAMES_IN_FLIGHT);
    }

    void ReinkanApp::CreateAOPipeline(DescriptorWrap descriptorWrap)
    {
        auto vertShaderCode = ReadFile("../shaders/ambientOcclusion.vert.spv");
        auto fragShaderCode = ReadFile("../shaders/ambientOcclusion.frag.spv");

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

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE; // enable sample shading in the pipeline
        multisampling.minSampleShading = 0.2f; // min fraction for sample shading; closer to one is smoother
        multisampling.rasterizationSamples = appMsaaSamples;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_FALSE;
        depthStencil.depthWriteEnable = VK_FALSE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;// BEWARE!!  NECESSARY!!
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

        std::vector<VkDynamicState> dynamicStates =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPushConstantRange pushConstantRanges = {
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantAO) };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorWrap.descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRanges;

        if (vkCreatePipelineLayout(appDevice, &pipelineLayoutInfo, nullptr, &appAOPipelineLayout) != VK_SUCCESS)
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
        pipelineInfo.layout = appAOPipelineLayout;
        pipelineInfo.renderPass = appAORenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(appDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &appAOPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(appDevice, fragShaderModule, nullptr);
        vkDestroyShaderModule(appDevice, vertShaderModule, nullptr);
    }

    void ReinkanApp::CreateAOBlurDescriptorSetWrap()
    {
        std::vector<VkDescriptorSetLayoutBinding> bindingTable;
        uint32_t bindingIndex = 0;

        // UBO
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                  bindingIndex++,                       // binding;
                                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,    // descriptorType;
                                  1,                                    // descriptorCount; 
                                  VK_SHADER_STAGE_COMPUTE_BIT });       // stageFlags;

        // AOImageWrap
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                       // binding;
                                      VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,     // descriptorType;
                                      2,                                    // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_COMPUTE_BIT });       // stageFlags;

        // appBlurAOMapImageWraps
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                       // binding;
                                      VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,     // descriptorType;
                                      2,                                    // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_COMPUTE_BIT });       // stageFlags;

        // appScanlineNormalImageWraps
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++,                       // binding;
                                      VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,     // descriptorType;
                                      2,                                    // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_COMPUTE_BIT });       // stageFlags;

        /*
        *
        * Binding Resources
        * - Gaussian Blur Array
        * - Source Images
        * - Destination Images
        *
        */
        appAOBlurDescriptorWrap.SetBindings(appDevice, bindingTable, MAX_FRAMES_IN_FLIGHT);

        bindingIndex = 0;
        appAOBlurDescriptorWrap.Write(appDevice, bindingIndex++, appAOBlurUBO);
        appAOBlurDescriptorWrap.Write(appDevice, bindingIndex++, appAORenderTargetImageWraps, MAX_FRAMES_IN_FLIGHT);
        appAOBlurDescriptorWrap.Write(appDevice, bindingIndex++, appBlurAOMapImageWraps, MAX_FRAMES_IN_FLIGHT); 
        appAOBlurDescriptorWrap.Write(appDevice, bindingIndex++, appScanlineNormalImageWraps, MAX_FRAMES_IN_FLIGHT);
    }   

    void ReinkanApp::CreateAOBlurHorizontalPipeline(DescriptorWrap descriptorWrap)
    {
        auto computeShaderCode = ReadFile("../shaders/aoBlurHorizontal.comp.spv");

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

        if (vkCreatePipelineLayout(appDevice, &pipelineLayoutInfo, nullptr, &appAOBlurHorizontalPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute pipeline layout!");
        }

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = appAOBlurHorizontalPipelineLayout;
        pipelineInfo.stage = computeShaderStageInfo;

        if (vkCreateComputePipelines(appDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &appAOBlurHorizontalPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute pipeline!");
        }

        vkDestroyShaderModule(appDevice, computeShaderModule, nullptr);
    }

    void ReinkanApp::CreateAOBlurVerticalPipeline(DescriptorWrap descriptorWrap)
    {
        auto computeShaderCode = ReadFile("../shaders/aoBlurVertical.comp.spv");

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

        if (vkCreatePipelineLayout(appDevice, &pipelineLayoutInfo, nullptr, &appAOBlurVerticalPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute pipeline layout!");
        }

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = appAOBlurVerticalPipelineLayout;
        pipelineInfo.stage = computeShaderStageInfo;

        if (vkCreateComputePipelines(appDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &appAOBlurVerticalPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute pipeline!");
        }

        vkDestroyShaderModule(appDevice, computeShaderModule, nullptr);
    }

    void ReinkanApp::CreateAOBlurResources()
    {
        // UBO [MAX_FRAMES_IN_FLIGHT]
        VkDeviceSize bufferSize = sizeof(AOBlurUniformBufferObject);
        appAOBlurUBO.resize(MAX_FRAMES_IN_FLIGHT);
        appAOBlurUBOMapped.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appAOBlurUBO[i] = CreateBufferWrap(bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkMapMemory(appDevice, appAOBlurUBO[i].memory, 0, bufferSize, 0, &appAOBlurUBOMapped[i]);
        }
    }

    void ReinkanApp::CreateAOBlurImageWraps()
    {
        // Blurred Shadow Map
        appBlurAOMapImageWraps.resize(MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appBlurAOMapImageWraps[i] = CreateImageWrap(appSwapchainExtent.width,
                appSwapchainExtent.height,
                VK_FORMAT_R32G32B32A32_SFLOAT,                                  // Image Format
                VK_IMAGE_TILING_OPTIMAL,                                        // Image Tilling
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT                             // As a result for render
                | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                | VK_IMAGE_USAGE_SAMPLED_BIT
                | VK_IMAGE_USAGE_STORAGE_BIT,                                   // Image Usage
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,                            // Memory Property
                1,
                appMsaaSamples);

            TransitionImageLayout(appBlurAOMapImageWraps[i].image,
                VK_FORMAT_R32G32B32A32_SFLOAT,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_GENERAL);

            appBlurAOMapImageWraps[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

            appBlurAOMapImageWraps[i].imageView = CreateImageView(appBlurAOMapImageWraps[i].image, VK_FORMAT_R32G32B32A32_SFLOAT);
            appBlurAOMapImageWraps[i].sampler = CreateImageSampler();
        }
    }

    void ReinkanApp::UpdateAOBlurUBO(uint32_t currentImage)
    {

        AOBlurUniformBufferObject ubo{};

        ubo.blurWidth = 100;
        ubo.screenWidth = appSwapchainExtent.width;
        ubo.screenHeight = appSwapchainExtent.height;

        // CPU to update buffer req: VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        memcpy(appAOBlurUBOMapped[currentImage], &ubo, sizeof(ubo));
    }

    void ReinkanApp::RecordAOPass(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = appAORenderPass;
        renderPassBeginInfo.framebuffer = appAOFrameBuffers[imageIndex]; // Output to Swapchain // screen
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = appSwapchainExtent;

        // Order match attachments
        std::array<VkClearValue, 1> clearValues{};
        clearValues[0].color = { {1.0f, 0.0f, 0.0f, 0.0f} };

        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, appAOPipeline);

            vkCmdBindDescriptorSets(commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                appAOPipelineLayout,
                0,
                1,
                &appAODescriptorWrap.descriptorSets[appCurrentFrame],
                0,
                nullptr);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(appSwapchainExtent.width);
            viewport.height = static_cast<float>(appSwapchainExtent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = { 0, 0 };
            scissor.extent = appSwapchainExtent;
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            PushConstantAO pushConstant;
            //pushConstant.cameraPosition = glm::vec4(appMainCamera->GetPosition(), 1.0);
            pushConstant.screenExtent = glm::vec2(appSwapchainExtent.width, appSwapchainExtent.height);
            pushConstant.sampleNumber = 20;
            pushConstant.aoRange = appAORange;
            pushConstant.debugFlag = appDebugFlag;
            pushConstant.aoScale = appAOScale;
            pushConstant.aoCurveK = appAOCurveK;
            pushConstant.debugFloat3 = appDebugFloat3;


            vkCmdPushConstants(commandBuffer,
                appAOPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PushConstantAO),
                &pushConstant
            );

            // Draw Imaginary Vertices
            /// Access vertex instances with gl_VertexIndex
            vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        }
        vkCmdEndRenderPass(commandBuffer);
    }

    void ReinkanApp::DestroyAOResources()
    {
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appAOBlurUBO[i].Destroy(appDevice);
        }

        // Descriptor Set
        appAODescriptorWrap.Destroy(appDevice);
        appAOBlurDescriptorWrap.Destroy(appDevice);

        // Pipeline
        vkDestroyPipeline(appDevice, appAOPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appAOPipelineLayout, nullptr);

        vkDestroyPipeline(appDevice, appAOBlurHorizontalPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appAOBlurHorizontalPipelineLayout, nullptr);

        vkDestroyPipeline(appDevice, appAOBlurVerticalPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appAOBlurVerticalPipelineLayout, nullptr);

        // RenderPass
        vkDestroyRenderPass(appDevice, appAORenderPass, nullptr);
    }

}