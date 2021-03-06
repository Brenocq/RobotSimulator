//--------------------------------------------------
// Robot Simulator
// fence.cpp
// Date: 2020-06-24
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <atta/graphics/vulkan/fence.h>
#include <atta/helpers/log.h>

namespace atta::vk
{
	Fence::Fence(std::shared_ptr<Device> device):
		_device(device)
	{

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if(vkCreateFence(_device->handle(), &fenceInfo, nullptr, &_fence) != VK_SUCCESS)
		{
			Log::error("Fence", "Failed to create fence!");
			exit(1);
		}
	}

	Fence::~Fence()
	{
		if(_fence != nullptr)
		{
			vkDestroyFence(_device->handle(), _fence, nullptr);
			_fence = nullptr;

		}
	}

	void Fence::reset()
	{
		if(vkResetFences(_device->handle(), 1, &_fence) != VK_SUCCESS)
		{
			Log::error("Fence", "Failed to reset fence!");
			exit(1);
		}
	}

	void Fence::wait(uint64_t timeout) const
	{
		if(vkWaitForFences(_device->handle(), 1, &_fence, VK_TRUE, timeout) != VK_SUCCESS)
		{
			Log::error("Fence", "Failed to wait for fence!");
			exit(1);
		}
	}
}
