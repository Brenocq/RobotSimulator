//--------------------------------------------------
// Atta Graphics
// envIrradiance.h
// Date: 2021-07-10
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef ATTA_GRAPHICS_VULKAN_COMPUTE_ENV_IRRADIANCE_H
#define ATTA_GRAPHICS_VULKAN_COMPUTE_ENV_IRRADIANCE_H

#include <memory>
#include <vector>

#include <atta/graphics/vulkan/vulkanCore.h>
#include <atta/graphics/vulkan/commandPool.h>
#include <atta/graphics/vulkan/shaderModule.h>
#include <atta/graphics/vulkan/descriptorSetManager.h>

namespace atta::vk::compute
{
	class EnvIrradiance
	{
		public:
			struct CreateInfo
			{
				std::shared_ptr<vk::Device> device;
				std::shared_ptr<vk::CommandPool> commandPool;
				std::shared_ptr<vk::Texture> input;
			};
			EnvIrradiance(CreateInfo info);
			~EnvIrradiance();

			void compute();
			std::shared_ptr<vk::Texture> getOutput() const { return _output; }

		private:
			std::shared_ptr<vk::Device> _device;
			std::shared_ptr<vk::CommandPool> _commandPool;
			std::shared_ptr<vk::Texture> _input;
			std::shared_ptr<vk::Texture> _output;

			std::shared_ptr<ShaderModule> _shaderModule;
			std::shared_ptr<DescriptorSetManager> _descriptorSetManager;
			VkPipelineLayout _pipelineLayout;
			VkPipeline _pipeline;
	};
}
#endif// ATTA_GRAPHICS_VULKAN_COMPUTE_ENV_IRRADIANCE_H
