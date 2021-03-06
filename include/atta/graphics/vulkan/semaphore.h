//--------------------------------------------------
// Robot Simulator
// semaphore.h
// Date: 2020-06-24
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef ATTA_GRAPHICS_VULKAN_SEMAPHORE_H
#define ATTA_GRAPHICS_VULKAN_SEMAPHORE_H

#include <iostream>
#include <string.h>
#include <atta/graphics/vulkan/device.h>

namespace atta::vk
{
	class Semaphore
	{
		public:
		Semaphore(std::shared_ptr<Device> device);
		~Semaphore();

		VkSemaphore handle() const { return _semaphore; }
		std::shared_ptr<Device> getDevice() const { return _device; }

		private:
		VkSemaphore _semaphore;
		std::shared_ptr<Device> _device;
	};
}

#endif// ATTA_GRAPHICS_VULKAN_SEMAPHORE_H
