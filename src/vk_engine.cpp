//> includes
#include "vk_engine.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <vk_initializers.h>
#include <vk_types.h>
#include <vk_images.h>

#include <VkBootstrap.h>

#include <chrono>
#include <thread>

VulkanEngine* loadedEngine = nullptr;

VulkanEngine& VulkanEngine::Get() { return *loadedEngine; }

constexpr bool bUseValidationLayers{ true };

void VulkanEngine::init()
{
	// only one engine initialization is allowed with the application.
	assert(loadedEngine == nullptr);
	loadedEngine = this;

	// We initialize SDL and create a window with it.
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

	_window = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		_windowExtent.width,
		_windowExtent.height,
		window_flags);

	// everything went fine
	_isInitialized = true;

	init_vulkan();

	init_swapchain();

	init_commands();

	init_sync_structures();

}


void VulkanEngine::draw()
{
	// Wait for the gpu to finish the last frame (time out of 1 second)
	//VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));  // Uses nanoseconds
	//// You have to reset a fence after using it
	//VK_CHECK(vkResetFences(_device, 1, &get_current_frame()._renderFence));

	// TODO: check why rwr0ong

	VK_CHECK(vkWaitForFences(_device, 1, &get_current_frame()._renderFence, true, 1000000000));
	VK_CHECK(vkResetFences(_device, 1, &get_current_frame()._renderFence));


	// Request the image index from the swapchain
	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(_device, _swapchain, 1000000000, get_current_frame()._swapchainSemaphore, nullptr, &swapchainImageIndex));

	// Only used for shorter names, mainCommandBuffer is a pointer to vulkan internals not handled by us.
	VkCommandBuffer cmd = get_current_frame()._mainCommandBuffer;

	// We are sure the command has finished executing, got this from the fence, so we can safely reset the buffer
	VK_CHECK(vkResetCommandBuffer(cmd, 0));

	// Begin the command buffer recording, we will use this command buffer once so we pass a signal bit to let vulkan know
	// Passing the bit lets the drivers slightly optimise the operations
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	VkClearColorValue clearValue;
	float flash = std::abs(std::sin(_frameNumber / 120.f));

	clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

	VkImageSubresourceRange clearRange = vkinit::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);

	// Clear the image
	vkCmdClearColorImage(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

	// Make the swapchain image into a presentable image
	vkutil::transition_image(cmd, _swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	// Finalise the command buffer to be sent off
	VK_CHECK(vkEndCommandBuffer(cmd));

	// Prepare the submission to queue
	// We wait on the _presentSemaphore, this semaphore is signalled when the swapchain is ready
	// Then we signal the _renderSemaphore to signal that the rendering is finished
	VkCommandBufferSubmitInfo cmdInfo = vkinit::command_buffer_submit_info(cmd);

	VkSemaphoreSubmitInfo waitInfo = vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, get_current_frame()._swapchainSemaphore);
	VkSemaphoreSubmitInfo signalInfo = vkinit::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, get_current_frame()._renderSemaphore);


	// TODO: ADD MORE COMMENTS
	VkSubmitInfo2 submit = vkinit::submit_info(&cmdInfo, &signalInfo, &waitInfo);

	VK_CHECK(vkQueueSubmit2(_graphicsQueue, 1, &submit, get_current_frame()._renderFence));

	VkPresentInfoKHR presentInfo = { .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, .pNext = nullptr };

	presentInfo.pSwapchains = &_swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &get_current_frame()._renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VK_CHECK(vkQueuePresentKHR(_graphicsQueue, &presentInfo));

	_frameNumber++;

	for (int i = 0; i < FRAME_OVERLAP; i++) {
		vkDestroyCommandPool(_device, get_current_frame()._commandPool, nullptr);

		vkDestroyFence(_device, _frames[i]._renderFence, nullptr);
		vkDestroySemaphore(_device, _frames[i]._renderSemaphore, nullptr);
		vkDestroySemaphore(_device, _frames[i]._swapchainSemaphore, nullptr);
	}

}

//void VulkanEngine::handle_input() {
//
//	const static Uint8* keyStates = SDL_GetKeyboardState(NULL);
//	bool keyPressedThisFrame = false;
//
//	// Test get continuous keyboard input
//	fmt::print("Key Pressed: ");
//
//	if (keyStates[SDL_SCANCODE_W]) {
//		fmt::print("W ");
//		keyPressedThisFrame = true;
//	}
//
//	fmt::print("\n");
//}

void VulkanEngine::run()
{
	SDL_Event e;
	bool bQuit = false;



	// main loop
	while (!bQuit) {
		// Handle events on queue
		while (SDL_PollEvent(&e) != 0) {
			// close the window when user alt-f4s or clicks the X button
			if (e.type == SDL_QUIT)
				bQuit = true;

			if (e.type == SDL_WINDOWEVENT) {
				if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
					stop_rendering = true;
				}
				if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
					stop_rendering = false;
				}
			}
		}


		// do not draw if we are minimized
		if (stop_rendering) {
			// throttle the speed to avoid the endless spinning
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		draw();
	}
}



void VulkanEngine::init_vulkan() {

	vkb::InstanceBuilder builder{};

	auto inst_ret{ builder.set_app_name("Example Vulkan App")
		.request_validation_layers(bUseValidationLayers)
		.use_default_debug_messenger()
		.require_api_version(1, 4, 0)
		.build() };

	vkb::Instance vkb_inst{ inst_ret.value() };

	_instance = vkb_inst.instance;
	_debug_messenger = vkb_inst.debug_messenger;


	// Surfaces represent the actual window of the app
	SDL_Vulkan_CreateSurface(_window, _instance, &_surface);

	// Supported features link to each other via the pNext member
	VkPhysicalDeviceVulkan14Features supported_features14{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = nullptr };
	VkPhysicalDeviceVulkan13Features supported_features13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &supported_features14 };
	VkPhysicalDeviceVulkan12Features supported_features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = &supported_features13 };
	VkPhysicalDeviceVulkan11Features supported_features11{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, .pNext = &supported_features12 };
	VkPhysicalDeviceFeatures2 supported_features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, .pNext = &supported_features12 };


	// Enable the features we want (ONLY ENABLE THE FEATURES YOU WILL USE OTHERWISE THE DRIVERS WILL INCUR ADDED OVERHEAD)
	supported_features13.dynamicRendering = true;
	supported_features13.synchronization2 = true;

	supported_features12.bufferDeviceAddress = true;
	supported_features12.descriptorIndexing = true;


	// Select the device to use for rendering (Handled by vkb)
	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice physicalDevice{ selector
		.set_minimum_version(1, 4)
		.set_required_features_14(supported_features14)
		.set_required_features_13(supported_features13)
		.set_required_features_12(supported_features12)
		.set_required_features_11(supported_features11)
		.set_surface(_surface)
		.select()
		.value() };

	// Create the device that Vulkan will use
	vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	vkb::Device vkbDevice{ deviceBuilder.build().value() };


	_device = vkbDevice.device;
	_chosenGPU = physicalDevice.physical_device;

	// Devices have been finished initialising

	// This creates the queue that will be used for all commands
	_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	// TODO: MISTYPED THIS

}

void VulkanEngine::init_commands()
{
	//create a command pool for commands submitted to the graphics queue.
	//we also want the pool to allow for resetting of individual command buffers
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	for (int i = 0; i < FRAME_OVERLAP; i++) {

		VK_CHECK(vkCreateCommandPool(_device, &commandPoolInfo, nullptr, &_frames[i]._commandPool));

		// allocate the default command buffer that we will use for rendering
		VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_frames[i]._commandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(_device, &cmdAllocInfo, &_frames[i]._mainCommandBuffer));
	}
}

void VulkanEngine::init_sync_structures() {
	VkFenceCreateInfo fenceCreateInfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::semaphore_create_info();


	// Create structs for each frame since writing to a struct thats being used by a currently being rendered frame is dangerous
	for (int i = 0; i < FRAME_OVERLAP; i++) {
		// Create the render fence
		VK_CHECK(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_frames[i]._renderFence));

		// Create the swapchain and render semaphore
		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._swapchainSemaphore));
		VK_CHECK(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_frames[i]._renderSemaphore));
	}


}

void VulkanEngine::create_swapchain(uint32_t width, uint32_t height) {
	vkb::SwapchainBuilder swapchainBuilder{ _chosenGPU, _device, _surface };
	_swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM; // This is a common format that most modern GPUs support

	vkb::Swapchain vkbSwapchain{ swapchainBuilder.set_desired_format(VkSurfaceFormatKHR{.format = _swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.set_desired_extent(width, height)
		.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		.build()
		.value() };


	// Store the details from the swapchain creation
	_swapchainExtent = vkbSwapchain.extent;
	_swapchain = vkbSwapchain.swapchain;
	_swapchainImages = vkbSwapchain.get_images().value();
	_swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void VulkanEngine::init_swapchain() {
	// This init is seperate from just creating the swapchain, we need to recreate the swapchain whenever the window resizes or moves
	create_swapchain(_windowExtent.width, _windowExtent.height);
}

void VulkanEngine::destroy_swapchain() {
	// First destroy the chain, which also deletes the images the chain holds
	vkDestroySwapchainKHR(_device, _swapchain, nullptr);

	// Next we destroy each view for the images
	for (int i = 0; i < _swapchainImageViews.size(); i++) {
		vkDestroyImageView(_device, _swapchainImageViews[i], nullptr);
	}
}

void VulkanEngine::cleanup()
{
	if (_isInitialized) {

		vkDeviceWaitIdle(_device);

		for (int i = 0; i < FRAME_OVERLAP; i++) {
			vkDestroyCommandPool(_device, _frames[i]._commandPool, nullptr);
		}


		// Kinda just destroy everything (order is important since they depend on each other)
		// We destroy in the inverse of creation:
		// SDL Window -> Instance -> Surface -> Device -> Swapchain -> Command Pool
		destroy_swapchain();

		vkDestroySurfaceKHR(_instance, _surface, nullptr);
		vkDestroyDevice(_device, nullptr);

		vkb::destroy_debug_utils_messenger(_instance, _debug_messenger);
		vkDestroyInstance(_instance, nullptr);
		SDL_DestroyWindow(_window);
	}
	else {
		fmt::println("ERR: Tried to close window but window wasn't initialised");
	}

	// clear engine pointer
	loadedEngine = nullptr;
}
