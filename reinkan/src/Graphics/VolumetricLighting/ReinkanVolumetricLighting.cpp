#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
	void ReinkanApp::CreateVLightingRenderPass()
	{
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
        colorAttachment.samples = appMsaaSamples;
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

        if (vkCreateRenderPass(appDevice, &renderPassInfo, nullptr, &appVLightRenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
	}

    VkVertexInputBindingDescription ReinkanApp::GetVLightBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VLightVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    std::array<VkVertexInputAttributeDescription, 2> ReinkanApp::GetVLightAttributeDescriptions()
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

    void ReinkanApp::CreateVLightDescriptorSetWrap()
    {
        std::vector<VkDescriptorSetLayoutBinding> bindingTable;
        uint32_t bindingIndex = 0;

        // UBO
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                  bindingIndex++, // 0                                                   // binding;
                                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                                // descriptorType;
                                  1,                                                                // descriptorCount; 
                                  VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT });     // stageFlags;

        // ShadowImageWrap
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++, // 1                                              // binding;
                                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,                    // descriptorType;
                                      MAX_FRAMES_IN_FLIGHT,                                         // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT }); // stageFlags;

        // ScanlineImageWrap
        bindingTable.emplace_back(VkDescriptorSetLayoutBinding{
                                      bindingIndex++, // 1                                              // binding;
                                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,                    // descriptorType;
                                      MAX_FRAMES_IN_FLIGHT,                                         // descriptorCount; // Has to > 0
                                      VK_SHADER_STAGE_FRAGMENT_BIT }); // stageFlags;

        appVLightDescriptorWrap.SetBindings(appDevice,
            bindingTable,
            MAX_FRAMES_IN_FLIGHT);

        bindingIndex = 0;

        appVLightDescriptorWrap.Write(appDevice, bindingIndex++, appScanlineUBO);
        appVLightDescriptorWrap.Write(appDevice, bindingIndex++, appShadowMapImageWraps, MAX_FRAMES_IN_FLIGHT);
        appVLightDescriptorWrap.Write(appDevice, bindingIndex++, appScanlinePositionImageWraps, MAX_FRAMES_IN_FLIGHT);
    }

	void ReinkanApp::CreateVLightPipeline(DescriptorWrap descriptorWrap)
	{
        auto vertShaderCode = ReadFile("../shaders/vlight.vert.spv");
        auto fragShaderCode = ReadFile("../shaders/vlight.frag.spv");


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

        auto bindingDescription = GetVLightBindingDescription();
        auto attributeDescriptions = GetVLightAttributeDescriptions();

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
        multisampling.sampleShadingEnable = VK_FALSE    ; // enable sample shading in the pipeline
        multisampling.minSampleShading = 0.2f; // min fraction for sample shading; closer to one is smoother
        multisampling.rasterizationSamples = appMsaaSamples;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_FALSE;
        depthStencil.depthWriteEnable = VK_FALSE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;// BEWARE!!  NECESSARY!!
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        //colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;

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
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantVLight) };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorWrap.descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRanges;

        if (vkCreatePipelineLayout(appDevice, &pipelineLayoutInfo, nullptr, &appVLightPipelineLayout) != VK_SUCCESS)
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
        pipelineInfo.layout = appVLightPipelineLayout;
        pipelineInfo.renderPass = appVLightRenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(appDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &appVLightPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(appDevice, fragShaderModule, nullptr);
        vkDestroyShaderModule(appDevice, vertShaderModule, nullptr);
	}

	void ReinkanApp::CreateVLightResources(size_t width, size_t height)
	{
        assert(width > 1 && height > 1, "VLight has given height map dimension <= 1");

        // Create Vertices
        appVLightVertices.resize(width * height);

#pragma omp parallel for
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                size_t index = i * width + j;

                float x = (static_cast<float>(j) / (width - 1.0f) - 0.5f) * 2.0f;

                float y = (static_cast<float>(i) / (height - 1.0f) - 0.5f) * 2.0f;

                appVLightVertices[index].position = glm::vec3(x, y, 1.0f);
                appVLightVertices[index].vertexNormal = glm::vec3(0, 0, -1.0f);
            }
        }

        // Add PeakVertex
        appVLightVertices.push_back({ glm::vec3(0, 0, -1), glm::vec3(0, 0, 0) });
        auto& peakVertex = appVLightVertices.back();
        unsigned int peakIndex = appVLightVertices.size() - 1;

        // Create Index Buffer on plane
        for (int i = 0; i < height - 1; ++i)
        {
            for (int j = 0; j < width - 1; ++j)
            {
                size_t index = i * width + j;

                appVLightIndices.push_back(index);
                appVLightIndices.push_back(index + 1);
                appVLightIndices.push_back(index + 1 + width);

                appVLightIndices.push_back(index);
                appVLightIndices.push_back(index + 1 + width);
                appVLightIndices.push_back(index + width);
            }
        }

        // Create Index Buffer on side of pyramid
        for (int i = 0; i < height - 1; ++i)
        {
            appVLightIndices.push_back(i * width);
            appVLightIndices.push_back((i + 1)*width);
            appVLightIndices.push_back(peakIndex);

            appVLightIndices.push_back((i + 1) * width + (width - 1));
            appVLightIndices.push_back(i * width + (width  - 1));
            appVLightIndices.push_back(peakIndex);
        }
        for (int j = 0; j < width - 1; ++j)
        {
            appVLightIndices.push_back(j + 1);
            appVLightIndices.push_back(j);
            appVLightIndices.push_back(peakIndex);

            unsigned int bottomBorder = width * (height - 1);

            appVLightIndices.push_back(j + bottomBorder);
            appVLightIndices.push_back(j + 1 + bottomBorder);
            appVLightIndices.push_back(peakIndex);
        }
        
        appVLightVertexBufferWrap = CreateStagedBufferWrap(appVLightVertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

        appVLightIndexBufferWrap = CreateStagedBufferWrap(appVLightIndices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	}

    void ReinkanApp::CreateVLightSpecialFunctionTable(size_t width, size_t height)
    {
        /*
        std::vector<std::vector<float>> specialFunctionTable;
        constexpr double halfPi = 3.1415926535 / 2.0;
        constexpr double e = 2.718281828459045;
        for (int i = 0; i < width; ++i)
        {
            // u = [0, 10]
            double iStep = 10.0 / static_cast<double>(width - 1);
            double u = i * iStep;
            for (int j = 0; j < height; ++j)
            {
                // v = [0, PI/2]
                double jStep = halfPi / static_cast<double>(width - 1);
                double v = v * jStep;
                specialFunctionTable[i][j] = pow(e, -u * tan(v));
            }
        }
        */
    }
}

