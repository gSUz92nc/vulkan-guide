#include <vk_pipelines.h>
#include <fstream>
#include <vk_initializers.h>

bool vkutil::load_shader_module(const char* filePath,
	VkDevice device, VkShaderModule* outShaderModule) {
	// Open the file 
	std::ifstream file{ filePath, std::ios::ate | std::ios::binary };

	if (!file.is_open()) {
		return false;
	}

	// We open the file with the cursor at the end "std::ios::ate" 
	// so we check the file size by looking at the location of the cursor
	// which gives the file size in bytes
	size_t fileSize = static_cast<size_t>(file.tellg());

	// SPIR-V expects the buffer to be in uint32_t so we reserve a vec with enough space for file
	std::vector<uint32_t> buffer{ static_cast<uint32_t>(fileSize / sizeof(uint32_t)) };

	file.seekg(0); // Move cursor to the start

	// Load the entire file into the buffer
	file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

	file.close();

	VkShaderModuleCreateInfo createInfo{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.codeSize = buffer.size() * sizeof(uint32_t),
		.pCode = buffer.data()
	};

	VkShaderModule shaderModule{};
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule)) {
		return false;
	}

	*outShaderModule = shaderModule;

	return true;
}
