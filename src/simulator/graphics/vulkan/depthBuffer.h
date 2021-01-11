//--------------------------------------------------
// Robot Simulator
// depthBuffer.h
// Date: 2020-07-08
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef ATTA_VK_DEPTH_BUFFER_H
#define ATTA_VK_DEPTH_BUFFER_H

#include <iostream>
#include <string.h>
#include "defines.h"
#include "device.h"
#include "commandPool.h"
#include "image.h"
#include "imageView.h"

namespace atta::vk
{
	class DepthBuffer
	{
		public:
			DepthBuffer(std::shared_ptr<Device> device, VkExtent2D extent);
			~DepthBuffer();

			std::shared_ptr<Device> getDevice() const { return _device; }
			VkFormat getFormat() const { return _format; }
			Image* getImage() const { return _image; }
			ImageView* getImageView() const { return _imageView; }

		private:
			VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

			std::shared_ptr<Device> _device;
			CommandPool* _commandPool;
			Image* _image;
			ImageView* _imageView;
			VkFormat _format;
			VkExtent2D _extent;
			bool _stencilAvailable;
	};
}

#endif// ATTA_VK_DEPTH_BUFFER_H
