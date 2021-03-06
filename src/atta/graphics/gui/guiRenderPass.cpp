//--------------------------------------------------
// Robot Simulator
// guiRenderPass.h
// Date: 2020-07-12
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <atta/graphics/gui/guiRenderPass.h>
#include <atta/helpers/log.h>

namespace atta
{
	GuiRenderPass::GuiRenderPass(std::shared_ptr<vk::Device> device, VkFormat colorFormat, VkFormat depthFormat):
		_device(device), _colorFormat(colorFormat), _depthFormat(depthFormat)
	{
		// For now the GUI will not have multisampling and images with VK_1_SAMPLE_BIT will be copy to the swapchain image
		bool useMultisampling = false;

		//----------- Color attachment ------------//
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = _colorFormat;
		colorAttachment.samples = useMultisampling?_device->getMsaaSamples():VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.finalLayout = useMultisampling ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		//----------- Depth attachment ------------//
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = _depthFormat;
		depthAttachment.samples = useMultisampling?_device->getMsaaSamples():VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		//----------- Resolve attachment ------------//
		VkAttachmentDescription colorAttachmentResolve{};
		VkAttachmentReference colorAttachmentResolveRef{};
		if(useMultisampling)
		{
			colorAttachmentResolve.format = _colorFormat;
			colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			colorAttachmentResolveRef.attachment = 2;
			colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		//----------- Subpass ------------//
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		if(useMultisampling)
			subpass.pResolveAttachments = &colorAttachmentResolveRef;

		//----------- SubpassDependecy ------------//
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		//----------- RenderPassInfo ------------//
		std::vector<VkAttachmentDescription> attachments = {colorAttachment, depthAttachment};
		if(useMultisampling)
			attachments.push_back(colorAttachmentResolve);

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if(vkCreateRenderPass(_device->handle(), &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS)
		{
			Log::error("GuiRenderPass", "Failed to create gui render pass!");
			exit(1);
		}
	}

	GuiRenderPass::~GuiRenderPass()
	{
		if(_renderPass != nullptr)
		{
			vkDestroyRenderPass(_device->handle(), _renderPass, nullptr);
			_renderPass = nullptr;
		}
	}
}
