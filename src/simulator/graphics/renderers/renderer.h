//--------------------------------------------------
// Atta Graphics
// renderer.h
// Date: 2021-01-11
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef ATTA_GRAPHICS_RENDERER_H
#define ATTA_GRAPHICS_RENDERER_H

#include <memory>
#include <vector>

#include "simulator/graphics/vulkan/image.h"
#include "simulator/graphics/vulkan/imageView.h"
#include "simulator/graphics/vulkan/vulkanCore.h"

namespace atta
{
	class Renderer
	{
		public:
			struct CreateInfo
			{
				std::shared_ptr<vk::VulkanCore> vkCore;
				float width;
				float height;
			};

			Renderer(CreateInfo info);

			virtual void render(VkCommandBuffer commandBuffer) = 0;

			//---------- Getters ----------//
			std::shared_ptr<vk::Image> getImage() const { return _image; }
			std::shared_ptr<vk::ImageView> getImageView() const { return _imageView; }

		protected:
			void createOutputImage();

			std::shared_ptr<vk::VulkanCore> _vkCore;
			VkExtent2D _extent;

			std::shared_ptr<vk::Image> _image;
			std::shared_ptr<vk::ImageView> _imageView;
	};
}
#endif// ATTA_GRAPHICS_RENDERER_H