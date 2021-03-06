//--------------------------------------------------
// Robot Simulator
// descriptorSets.cpp
// Date: 2020-07-07
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <atta/graphics/vulkan/descriptorSets.h>
#include <atta/graphics/vulkan/helpers.h>
#include <atta/helpers/log.h>

namespace atta::vk
{
	DescriptorSets::DescriptorSets(
			std::shared_ptr<Device> device, 
			std::shared_ptr<DescriptorPool> descriptorPool, 
			std::shared_ptr<DescriptorSetLayout> descriptorSetLayout,
			std::map<uint32_t, VkDescriptorType> bindingTypes, 
			size_t size):
		_device(device), 
		_descriptorPool(descriptorPool),
		_descriptorSetLayout(descriptorSetLayout),
		_bindingTypes(bindingTypes)
	{
		std::vector<VkDescriptorSetLayout> layouts(size, _descriptorSetLayout->handle());

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPool->handle();
		allocInfo.descriptorSetCount = static_cast<uint32_t>(size);
		allocInfo.pSetLayouts = layouts.data();

		_descriptorSets.resize(size);

		if(vkAllocateDescriptorSets(_device->handle(), &allocInfo, _descriptorSets.data()) != VK_SUCCESS)
		{
			Log::error("DescriptorSets", "Failed to allocate descriptor sets!");
			exit(1);
		}
	}

	DescriptorSets::~DescriptorSets()
	{
	}

	VkWriteDescriptorSet DescriptorSets::bind(const uint32_t index, const uint32_t binding, const VkDescriptorBufferInfo& bufferInfo, const uint32_t count) const
	{
		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = _descriptorSets[index];
		descriptorWrite.dstBinding = binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = getBindingType(binding);
		descriptorWrite.descriptorCount = count;
		descriptorWrite.pBufferInfo = &bufferInfo;

		return descriptorWrite;
	}

	VkWriteDescriptorSet DescriptorSets::bind(const uint32_t index, const uint32_t binding, const VkDescriptorImageInfo& imageInfo, const uint32_t count) const
	{
		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = _descriptorSets[index];
		descriptorWrite.dstBinding = binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = getBindingType(binding);
		descriptorWrite.descriptorCount = count;
		descriptorWrite.pImageInfo = &imageInfo;

		return descriptorWrite;
	}

	VkWriteDescriptorSet DescriptorSets::bind(uint32_t index, uint32_t binding, const VkWriteDescriptorSetAccelerationStructureKHR& structureInfo, const uint32_t count) const
	{
		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = _descriptorSets[index];
		descriptorWrite.dstBinding = binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = getBindingType(binding);
		descriptorWrite.descriptorCount = count;
		descriptorWrite.pNext = &structureInfo;

		return descriptorWrite;
	}

	VkDescriptorType DescriptorSets::getBindingType(uint32_t binding) const
	{
		const auto it = _bindingTypes.find(binding);
		if (it == _bindingTypes.end())
		{
			Log::error("DescriptorSets", "Binding not found!");
			exit(1);
		}

		return it->second;
	}

	void DescriptorSets::updateDescriptors(uint32_t index, const std::vector<VkWriteDescriptorSet>& descriptorWrites)
	{
		vkUpdateDescriptorSets(
			_device->handle(),
			static_cast<uint32_t>(descriptorWrites.size()),
			descriptorWrites.data(), 0, nullptr);
	}
}
