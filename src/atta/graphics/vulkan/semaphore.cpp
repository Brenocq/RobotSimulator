//--------------------------------------------------
// Robot Simulator
// semaphore.cpp
// Date: 2020-06-24
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <atta/graphics/vulkan/semaphore.h>
#include <atta/helpers/log.h>

namespace atta::vk
{
	Semaphore::Semaphore(std::shared_ptr<Device> device):
		_device(device)
	{

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if(vkCreateSemaphore(_device->handle(), &semaphoreInfo, nullptr, &_semaphore) != VK_SUCCESS)
		{
			Log::error("Semaphore", "Failed to create semaphore!");
			exit(1);
		}
	}

	Semaphore::~Semaphore()
	{
		if(_semaphore != nullptr)
		{
			vkDestroySemaphore(_device->handle(), _semaphore, nullptr);
			_semaphore = nullptr;

		}
	}
}
