//--------------------------------------------------
// Robot Simulator
// stagingBuffer.cpp
// Date: 2020-07-06
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <atta/graphics/vulkan/stagingBuffer.h>

namespace atta::vk
{
	StagingBuffer::StagingBuffer(std::shared_ptr<Device> device, void* dataToMap, VkDeviceSize size):
		Buffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	{
		void* data;
		vkMapMemory(_device->handle(), _bufferMemory, 0, _bufferInfo.size, 0, &data);
			memcpy(data, dataToMap, static_cast<size_t>(_bufferInfo.size));
		vkUnmapMemory(_device->handle(), _bufferMemory);
	}

	//template <typename T>
	//StagingBuffer::StagingBuffer(std::shared_ptr<Device> device, std::vector<T>& content):
	//	Buffer(device, sizeof(content[0])*content.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	//{
	//	void* data;
	//	vkMapMemory(_device->handle(), _bufferMemory, 0, _bufferInfo.size, 0, &data);
	//		memcpy(data, content.data(), static_cast<size_t>(_bufferInfo.size));
	//	vkUnmapMemory(_device->handle(), _bufferMemory);
	//}

	StagingBuffer::~StagingBuffer()
	{

	}

	void StagingBuffer::mapFromData(std::shared_ptr<Device> device, void* dataToMap, VkDeviceSize size)
	{
		void* data;
		vkMapMemory(_device->handle(), _bufferMemory, 0, _bufferInfo.size, 0, &data);
			memcpy(data, dataToMap, static_cast<size_t>(_bufferInfo.size));
		vkUnmapMemory(_device->handle(), _bufferMemory);
	}
}
