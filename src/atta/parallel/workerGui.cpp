//--------------------------------------------------
// Atta Parallel
// workerGui.cpp
// Date: 2021-01-07
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <atta/parallel/workerGui.h>
#include <iostream>
#include <atta/helpers/log.h>
#include <atta/helpers/drawer.h>
#include <atta/helpers/evaluator.h>
#include <atta/graphics/vulkan/imageMemoryBarrier.h>
#include <atta/graphics/renderers/rastRenderer/rastRenderer.h>
#include <atta/graphics/renderers/rayTracing/rayTracingVulkan/rayTracing.h>
#include <atta/graphics/renderers/renderer2D/renderer2D.h>

namespace atta
{
	const int MAX_FRAMES_IN_FLIGHT = 2;
	WorkerGui::WorkerGui(CreateInfo info):
		_vkCore(info.vkCore), _scene(info.scene), _currentFrame(0), _mainRendererIndex(-1), _cameraUpdated(false),
		_cameraControlType(info.cameraControlType), _guiRenderer(info.guiRenderer),
		_mouseInsideViewport(false),
		_runBeforeWorkerGuiRender(info.runBeforeWorkerGuiRender), 
		_handleKeyboard(info.handleKeyboard), _handleMousePosition(info.handleMousePosition),
		_handleMouseButton(info.handleMouseButton), _handleMouseScroll(info.handleMouseScroll)
	{
		// Create window (GUI thread only)
		_window = std::make_shared<Window>();
		_window->windowResized = [this](const int width, const int height){ onWindowResized(width, height); };
		_window->onKey = [this](const int key, const int scancode, const int action, const int mods) { onKey(key, scancode, action, mods); };
		_window->onCursorPosition = [this](const double xpos, const double ypos) { onCursorPosition(xpos, ypos); };
		_window->onMouseButton = [this](const int button, const int action, const int mods) { onMouseButton(button, action, mods); };
		_window->onScroll = [this](const double xoffset, const double yoffset) { onScroll(xoffset, yoffset); };

		// Create model view controller
		_modelViewController = std::make_shared<ModelViewController>(
				_cameraControlType == CAMERA_CONTROL_TYPE_2D?
					ModelViewController::CONTROL_TYPE_2D:
					ModelViewController::CONTROL_TYPE_3D
					);

		// Vulkan objects
		_surface = std::make_shared<vk::Surface>(_vkCore->getInstance(), _window);
		_swapChain = std::make_shared<vk::SwapChain>(_vkCore->getDevice(), _surface);
		_commandPool = std::make_shared<vk::CommandPool>(_vkCore->getDevice(), vk::CommandPool::DEVICE_QUEUE_FAMILY_GRAPHICS, vk::CommandPool::QUEUE_GUI, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		_commandBuffers = std::make_shared<vk::CommandBuffers>(_vkCore->getDevice(), _commandPool, _swapChain->getImages().size());

		// Create render sync objects
		_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        _imagesInFlight.resize(_swapChain->getImages().size(), VK_NULL_HANDLE);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			_imageAvailableSemaphores[i] = std::make_shared<vk::Semaphore>(_vkCore->getDevice());
			_renderFinishedSemaphores[i] = std::make_shared<vk::Semaphore>(_vkCore->getDevice());
			_inFlightFences[i] = std::make_shared<vk::Fence>(_vkCore->getDevice());
		}

		// Create user interface objects
		UserInterface::CreateInfo uiInfo =
		{
			.device = _vkCore->getDevice(),
			.window = _window,
			.scene = _scene,
			.swapChain = _swapChain
		};
		_ui = std::make_shared<UserInterface>(uiInfo);

		createRenderers();
	}

	WorkerGui::~WorkerGui()
	{
		_commandPool->waitCompletion();
	}

	void WorkerGui::operator()()
	{
		while(!_window->shouldClose() && !_shouldFinish && !_ui->shouldClose())
		{
			if(_runBeforeWorkerGuiRender)
				_runBeforeWorkerGuiRender(this);
			_cameraUpdated = _modelViewController->updateCamera(0.1);
			render();
			_window->poolEvents();
		}
		// Send signal to close atta simulator
		_shouldFinish = true;
	}


	void WorkerGui::createRenderers()
	{ 
		if(_guiRenderer == GUI_RENDERER_RAST)
		{
			// Create rasterization render
			RastRenderer::CreateInfo rastRendInfo = {
				.vkCore = _vkCore,
				.commandPool = _commandPool,
				.width = 1200,
				.height = 900,
				.fov = 60,
				.scene = _scene,
				.viewMat = atta::lookAt(vec3(1,1,1), vec3(0,0,0), vec3(0,1,0)),
			};
			std::shared_ptr<RastRenderer> rast = std::make_shared<RastRenderer>(rastRendInfo);
			_renderers.push_back(std::static_pointer_cast<Renderer>(rast));
			_ui->addRenderer("mainRast", _renderers.back());
		} 
		else if(_guiRenderer == GUI_RENDERER_RAY_TRACING_VULKAN)
		{
			rt::vk::RayTracing::CreateInfo rtVkRendInfo = 
			{
				.vkCore = _vkCore,
				.commandPool = _commandPool,
				.width = 1200,
				.height = 900,
				.scene = _scene,
				.viewMat = atta::lookAt(vec3(1,1,1), vec3(0,0,0), vec3(0,1,0)),
				.projMat = atta::perspective(atta::radians(60), 1200.0/900, 0.01f, 1000.0f)
			};
			std::shared_ptr<rt::vk::RayTracing> rtVk = std::make_shared<rt::vk::RayTracing>(rtVkRendInfo);
			_renderers.push_back(std::static_pointer_cast<Renderer>(rtVk));
			_ui->addRenderer("mainRT", _renderers.back());
		}
		else if(_guiRenderer == GUI_RENDERER_2D)
		{
			Renderer2D::CreateInfo rend2DInfo = {
				.vkCore = _vkCore,
				.commandPool = _commandPool,
				.width = 1200,
				.height = 900,
				.scene = _scene
			};
			std::shared_ptr<Renderer2D> renderer2D = std::make_shared<Renderer2D>(rend2DInfo);
			_renderers.push_back(std::static_pointer_cast<Renderer>(renderer2D));
			_ui->addRenderer("main2D", _renderers.back());
		}
		else
		{
			Log::warning("WorkerGui", "This renderer is not implemented yet");
		}

		if(_renderers.size()>0)
		{
			_mainRendererIndex = _renderers.size()-1;
			// Start model view controller view matrix from main renderer
			_modelViewController->reset(_renderers[_mainRendererIndex]->getViewMatrix());
		}
	}

	void WorkerGui::render()
	{
		//LocalEvaluator eval("time per frame");

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(_vkCore->getDevice()->handle(), _swapChain->handle(), 
				UINT64_MAX, _imageAvailableSemaphores[_currentFrame]->handle(), VK_NULL_HANDLE, &imageIndex);

		//---------- Check swapchain ----------//
		if(result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			// Recreate the swapchain (window resized)
			Log::warning("WorkerGui", "Swap chain out of date! Must reset the swapChain!");
			return;
		}
		else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			Log::error("WorkerGui", "Failed to acquire swap chain image!");
			exit(1);
		}

		// Update main renderer camera
		if(_cameraUpdated)
		{
			if(_renderers.size()>0)
				_renderers[_mainRendererIndex]->updateCameraMatrix(_modelViewController->getModelView());
			if(_renderers[_mainRendererIndex]->getType()==RENDERER_TYPE_2D)
			{
				// Update projection matrix with scale (mouse scroll)
				std::shared_ptr<Renderer2D> rend2D = std::static_pointer_cast<Renderer2D>(_renderers[_mainRendererIndex]);
				float totalScroll = _modelViewController->collectTotalScroll();
				float orthoHeight = rend2D->getOrthoHeight();
				float newOrthoHeight = orthoHeight-totalScroll/10;
				if(newOrthoHeight<0.1)
					newOrthoHeight = 0.1;
				rend2D->setOrthoHeight(newOrthoHeight);
				_modelViewController->setSpeed(_modelViewController->getSpeed()*(newOrthoHeight/orthoHeight));
			}
			_cameraUpdated = false;
		}

		//---------- Record to command buffer ----------//
		VkCommandBuffer commandBuffer = _commandBuffers->begin(imageIndex);
		{
			recordCommands(commandBuffer, imageIndex);
		}
		_commandBuffers->end(imageIndex);

		//---------- CPU-GPU syncronization ----------//
		_inFlightFences[_currentFrame]->wait(UINT64_MAX);
		_inFlightFences[_currentFrame]->reset();

		//---------- GPU-GPU syncronization ----------//
		VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]->handle()};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]->handle()};

		//---------- Submit to graphics queue ----------//
		std::array<VkCommandBuffer, 1> submitCommandBuffers = { _commandBuffers->handle()[imageIndex] };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;// Wait image available
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = static_cast<uint32_t>(submitCommandBuffers.size());
		submitInfo.pCommandBuffers = submitCommandBuffers.data();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;// Signal image rendered

		for(auto texture : Texture::textureInfos())
		{
			if(auto vkTex = texture.vkTexture.lock())
			{
				vkTex->lock();
			}
		}

		if(vkQueueSubmit(_vkCore->getDevice()->getGraphicsQueueGUI(), 1, &submitInfo, _inFlightFences[_currentFrame]->handle()) != VK_SUCCESS)
		{
			Log::error("WorkerGui", "Failed to submit draw command buffer!");
			exit(1);
		}

		for(auto texture : Texture::textureInfos())
		{
			if(auto vkTex = texture.vkTexture.lock())
			{
				vkTex->unlock();
			}
		}

		//---------- Submit do present queue ----------//
		VkSwapchainKHR swapChains[] = {_swapChain->handle()};
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;// Wait image rendered
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(_vkCore->getDevice()->getPresentQueueGUI(), &presentInfo);
		if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			// TODO update frameBufferResized variable
			Log::warning("WorkerGui", "Framebuffer resized! Must reset the swapChain!");
		} 
		else if(result != VK_SUCCESS) 
		{
			Log::error("WorkerGui", "Failed to present swap chain image!");
			exit(1);
		}

		//---------- Next frame ----------//
		_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void WorkerGui::recordCommands(VkCommandBuffer commandBuffer, unsigned imageIndex)
	{
		VkImageSubresourceRange subresourceRange;
		subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = 1;

		//---------- Render user interface ----------//
		// The ui resolve subpass change the layout to present
		// Render renderers and copy their images if necessary
		// It also updates the renderer extent if necessary
		_ui->render(commandBuffer, imageIndex);
	}

	//------------------------------------------------------------------------------//
	//------------------------------ Window Callbacks ------------------------------//
	//------------------------------------------------------------------------------//
	void WorkerGui::onWindowResized(int width, int height)
	{
		// TODO recreate renderers
		_ui->onWindowResized(width, height);
	}

	void WorkerGui::onKey(int key, int scancode, int action, int mods)
	{
		switch(key)
		{
			case GLFW_KEY_ESCAPE:
				_window->close();
				break;
			case GLFW_KEY_R:
				if(action == GLFW_PRESS)
				{
					// TODO Update _mainRendererIndex on gui thread?
					//_mainRendererIndex = _mainRendererIndex?0:1;
					//Log::debug("WorkerGui", "Changed main to $0", _mainRendererIndex);
				}
				break;
		}

		_modelViewController->onKey(key, scancode, action, mods);
		_ui->onKey(key, scancode, action, mods);
		if(_handleKeyboard) _handleKeyboard(key, action);
	}

	void WorkerGui::onCursorPosition(double xpos, double ypos)
	{
		_modelViewController->onCursorPosition(xpos, ypos);
		_ui->onCursorPosition(xpos, ypos);

		// TODO hardcoded offsets
		float xposViewport = xpos-(35+2*3);
		float yposViewport = ypos-(26+22);
		VkExtent2D extent = _ui->getViewportRenderer()->getImage()->getExtent();
		xposViewport/=extent.width;
		yposViewport/=extent.height;

		_mouseInsideViewport = xposViewport>=0 && xposViewport<=1 && yposViewport>=0 && yposViewport<=1;
		if(_handleMousePosition && _mouseInsideViewport) _handleMousePosition(xposViewport, yposViewport);
	}

	void WorkerGui::onMouseButton(int button, int action, int mods)
	{
		switch(button)
		{
			case GLFW_MOUSE_BUTTON_MIDDLE:
				_window->toggleCursorVisibility();
				break;
		}

		_modelViewController->onMouseButton(button, action, mods);
		_ui->onMouseButton(button, action, mods);
		if(_handleMouseButton && _mouseInsideViewport) _handleMouseButton(button, action);
	}

	void WorkerGui::onScroll(double xoffset, double yoffset)
	{
		_modelViewController->onScroll(xoffset, yoffset);
		_ui->onScroll(xoffset, yoffset);
		if(_handleMouseScroll && _mouseInsideViewport) _handleMouseScroll(xoffset, yoffset);
	}
}
