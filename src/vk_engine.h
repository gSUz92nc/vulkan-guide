// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vk_descriptors.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

// Note: This implementation is inefficient at scale since we are storing whole 
// std::functions for every object we are deleting. Better implementations would store
// an array of vulkan handles of various types such as VkImage, VkBuffer, etc.
// https://vkguide.dev/docs/new_chapter_2/vulkan_new_rendering/
struct DeletionQueue {
	std::deque<std::function<void()>> deletors;

	// Push the function that handles deleting the 
	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)(); // Call each function in the deletors
		}
	}
};

struct FrameData {
	VkCommandPool _commandPool{};
	VkCommandBuffer _mainCommandBuffer{};

	// Semaphores are used for synchronisation between the CPU and the GPU

	// TODO: NOTE(Validation): The tutorial I used has 1 swapchain semaphore per frame
	// This triffers VUID-vkQueueSubmit2-semaphore-03868 when validation layers are on
	// Because the swapchain can recycle the same semaphore before presentation finishes
	// To fix this cleanly later see: https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html

	VkSemaphore _swapchainSemaphore{}; // This is going to be used making render commands wait on swapchain image requests 
	VkSemaphore _renderSemaphore{}; // This is used for presenting the image to the OS 
	// Fences are used for preventing work from being run on the GPU before another task as finished
	VkFence _renderFence{}; // This lets us wait for the draw commands of a given frame to finish

	DeletionQueue _deletionQueue{};
};

struct ComputePushConstants {
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};

struct ComputeEffect {
	const std::string name{};

	VkPipeline pipeline;
	VkPipelineLayout layout;

	ComputePushConstants data;
};

constexpr unsigned int FRAME_OVERLAP = 2; // This is set to two for doubl-buffering

class VulkanEngine {
public:

	VkFence _immFence{};
	VkCommandBuffer _immCommandBuffer{};
	VkCommandPool _immCommandPool{};

	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
	VkExtent2D _windowExtent{ 1700 , 800 };

	VkInstance _instance; // Vulkan library handle
	VkDebugUtilsMessengerEXT _debug_messenger{}; // Vulkan debug output handle
	VkPhysicalDevice _chosenGPU{}; // The GPU we will be doing rendering on
	VkDevice _device{}; // The device that lets us run commands on the chosen GPU (Used for multiple program access to the same GPU)
	VkSurfaceKHR _surface{}; // Where the image will be presented 

	VkSwapchainKHR _swapchain{};
	VkFormat _swapchainImageFormat{};

	std::vector<VkImage> _swapchainImages{};
	std::vector<VkImageView> _swapchainImageViews{};

	FrameData _frames[FRAME_OVERLAP];
	FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

	// Draw resources
	AllocatedImage _drawImage{};
	VkExtent2D _drawExtent{};

	DeletionQueue _mainDeletionQueue{};

	DescriptorAllocator globalDescriptorAllocator{};

	// Pipelines
	VkPipeline _gradientPipeline{};
	VkPipelineLayout _gradientPipelineLayout{};

	VkPipelineLayout m_trianglePipelineLayout{};
	VkPipeline m_trianglePipeline{};

	VkDescriptorSet _drawImageDescriptors{};
	VkDescriptorSetLayout _drawImageDescriptorLayout{};

	VkQueue _graphicsQueue{};
	uint32_t _graphicsQueueFamily{};

	VkExtent2D _swapchainExtent{};

	VmaAllocator _allocator{};

	struct SDL_Window* _window{ nullptr };

	static VulkanEngine& Get();


	// Used for sending commands to the GPU independant of the swapchain or rendering logic
	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);

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

	void init_descriptors();

	void init_pipelines();

	void init_background_pipelines();

	void init_triangle_pipelines();

	void init_imgui();

	void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);

	void draw_background(VkCommandBuffer cmd);

	void draw_geometry(VkCommandBuffer cmd);

	void create_swapchain(uint32_t width, uint32_t height);

	void destroy_swapchain();
};
