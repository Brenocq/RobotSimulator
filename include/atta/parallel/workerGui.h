//--------------------------------------------------
// Atta Parallel
// workerGui.h
// Date: 2021-01-07
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef ATTA_PARALLEL_WORKER_GUI_H
#define ATTA_PARALLEL_WORKER_GUI_H

#include "worker.h"
#include <functional>
#include <atta/core/scene.h>
#include <atta/core/common.h>
#include <atta/graphics/core/window.h>
#include <atta/graphics/core/modelViewController.h>
#include <atta/graphics/vulkan/vulkanCore.h>
#include <atta/graphics/vulkan/surface.h>
#include <atta/graphics/vulkan/swapChain.h>
#include <atta/graphics/vulkan/commandPool.h>
#include <atta/graphics/vulkan/commandBuffers.h>
#include <atta/graphics/vulkan/semaphore.h>
#include <atta/graphics/vulkan/fence.h>
#include <atta/graphics/vulkan/image.h>
#include <atta/graphics/renderers/renderer.h>
#include <atta/graphics/gui/userInterface.h>

namespace atta
{
	class WorkerGui : public Worker
	{
		public:
			struct ImageCopy {
				std::shared_ptr<vk::Image> image;
				VkExtent2D extent;
				VkOffset2D offset;
			};

			enum CameraControlType
			{
				CAMERA_CONTROL_TYPE_2D,
				CAMERA_CONTROL_TYPE_3D
			};

			struct CreateInfo {
				std::shared_ptr<vk::VulkanCore> vkCore;
				std::shared_ptr<Scene> scene;
				CameraControlType cameraControlType;
				GuiRenderer guiRenderer;
				std::function<void(WorkerGui*)> runBeforeWorkerGuiRender;
				std::function<void(int key, int action)> handleKeyboard;
				std::function<void(double xpos, double ypos)> handleMousePosition;
				std::function<void(int button, int action)> handleMouseButton;
				std::function<void(double xoffset, double yoffset)> handleMouseScroll;
			};

			WorkerGui(CreateInfo info);
			~WorkerGui();

			void operator()();

			void createRenderers();

			//---------- Setters ----------//
			void setCommands(std::vector<std::function<void(VkCommandBuffer commandBuffer)>> commands) { _commands = commands; };
			//---------- Getters ----------//
			std::shared_ptr<ModelViewController> getModelViewController() const { return _modelViewController; }
			std::vector<std::shared_ptr<Renderer>> getRenderers() const { return _renderers; }
			std::shared_ptr<Renderer> getMainRenderer() const { return _renderers[_mainRendererIndex]; }

		private:
			void render();
			void recordCommands(VkCommandBuffer commandBuffer, unsigned imageIndex);

			// Window callbacks
			void onWindowResized(int width, int height);
			void onKey(int key, int scancode, int action, int mods);
			void onCursorPosition(double xpos, double ypos);
			void onMouseButton(int button, int action, int mods);
			void onScroll(double xoffset, double yoffset);

			// Core objects
			std::shared_ptr<Window> _window;
			std::shared_ptr<ModelViewController> _modelViewController;
			std::shared_ptr<vk::VulkanCore> _vkCore;
			std::shared_ptr<Scene> _scene;

			std::shared_ptr<vk::Surface> _surface;
			std::shared_ptr<vk::SwapChain> _swapChain;
			std::shared_ptr<vk::CommandPool> _commandPool;
			std::shared_ptr<vk::CommandBuffers> _commandBuffers;

			// Presentation sync objects
			std::vector<std::shared_ptr<vk::Semaphore>> _imageAvailableSemaphores;
			std::vector<std::shared_ptr<vk::Semaphore>> _renderFinishedSemaphores;
			std::vector<std::shared_ptr<vk::Fence>> _inFlightFences;
			std::vector<VkFence> _imagesInFlight;
			int _currentFrame;

			// Renderers
			std::vector<std::shared_ptr<Renderer>> _renderers;
			int _mainRendererIndex;// Renderer that the user will use to see the world
			bool _cameraUpdated;// Used mainly by ray tracing renderers to create acceleration structures
			CameraControlType _cameraControlType;
			GuiRenderer _guiRenderer;

			// Work to process
			std::vector<std::function<void(VkCommandBuffer commandBuffer)>> _commands;

			// Images to copy (TODO show renderers images with guib)
			std::vector<ImageCopy> _imageCopies;

			// User interface
			std::shared_ptr<UserInterface> _ui;

			// User functions
			bool _mouseInsideViewport;
			std::function<void(WorkerGui*)> _runBeforeWorkerGuiRender;
			std::function<void(int key, int action)> _handleKeyboard;
			std::function<void(double xpos, double ypos)> _handleMousePosition;
			std::function<void(int button, int action)> _handleMouseButton;
			std::function<void(double xoffset, double yoffset)> _handleMouseScroll;
	};
}

#endif// ATTA_PARALLEL_WORKER_GUI_H
