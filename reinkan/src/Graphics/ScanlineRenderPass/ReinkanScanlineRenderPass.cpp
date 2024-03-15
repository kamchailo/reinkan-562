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
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_NONE;
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
}
