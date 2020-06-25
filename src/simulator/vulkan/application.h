//--------------------------------------------------
// Robot Simulator
// application.h
// Date: 21/06/2020
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>

#include "window.h"
#include "instance.h"
#include "debugMessenger.h"
#include "physicalDevice.h"
#include "device.h"
#include "surface.h"
#include "swapChain.h"
#include "graphicsPipeline.h"
#include "frameBuffer.h"
#include "commandPool.h"
#include "commandBuffers.h"
#include "semaphore.h"

class Application
{
	public:
	Application();
	~Application();

	void run();

	private:
	void drawFrame();
	void render(int i);

	Window* _window;
	Instance* _instance;
	DebugMessenger* _debugMessenger;
	PhysicalDevice* _physicalDevice;
	Device* _device;
	Surface* _surface;
	SwapChain* _swapChain;
	GraphicsPipeline* _graphicsPipeline;
	CommandPool* _commandPool;
	CommandBuffers* _commandBuffers;

	std::vector<FrameBuffer*> _frameBuffers;

	Semaphore* _imageAvailableSemaphore;
	Semaphore* _renderFinishedSemaphore;
};

#endif// APPLICATION_H