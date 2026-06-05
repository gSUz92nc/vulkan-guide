// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>


struct FrameData {
	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	// Semaphores are used for synchronisation between the CPU and the GPU
	VkSemaphore _swapchainSemaphore; // This is going to be used making render commands wait on swapchain image requests 
	VkSemaphore _renderSemaphore; // This is used for presenting the image to the OS 
	// Fences are used for preventing work from being run on the GPU before another task as finished
	VkFence _renderFence; // This lets us wait for the draw commands of a given frame to finish
};

constexpr unsigned int FRAME_OVERLAP = 2; // This is set to two for doubl-buffering

class VulkanEngine {
public:

	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
	VkExtent2D _windowExtent{ 1700 , 800 };

	VkInstance _instance; // Vulkan library handle
	VkDebugUtilsMessengerEXT _debug_messenger; // Vulkan debug output handle
	VkPhysicalDevice _chosenGPU; // The GPU we will be doing rendering on
	VkDevice _device; // The device that lets us run commands on the chosen GPU (Used for multiple program access to the same GPU)
	VkSurfaceKHR _surface; // Where the image will be presented 

	VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;

	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;

	FrameData _frames[FRAME_OVERLAP];
	FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;

	VkExtent2D _swapchainExtent;

	struct SDL_Window* _window{ nullptr };

	static VulkanEngine& Get();

	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();

private:
	
	void init_vulkan();

	void init_swapchain();

	void init_commands();

	void init_sync_structures();

	void create_swapchain(uint32_t width, uint32_t height);

	void destroy_swapchain();
};
