#include <vk_pipelines.h>
#include <fstream>
#include <vk_initializers.h>

static constexpr VkPipelineDepthStencilStateCreateInfo depth_disabled_state() noexcept {
	return {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0,
		.depthTestEnable = VK_FALSE,
		.depthWriteEnable = VK_FALSE,
		.depthCompareOp = VK_COMPARE_OP_ALWAYS,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE,
		.front = {},
		.back = {},
		.minDepthBounds = 0.0f,
		.maxDepthBounds = 1.0f,
	};
}

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

	fmt::println("File size: {}", fileSize);

	// SPIR-V expects the buffer to be in uint32_t so we reserve a vec with enough space for file
	std::vector<uint32_t> buffer(static_cast<uint32_t>(fileSize / sizeof(uint32_t)));

	file.seekg(0); // Move cursor to the start

	// Load the entire file into the buffer
	file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

	file.close();

	VkShaderModuleCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		.codeSize = buffer.size() * sizeof(uint32_t),
		.pCode = buffer.data()
	};

	VkShaderModule shaderModule{};
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		return false;
	}

	*outShaderModule = shaderModule;

	return true;
}

void PipelineBuilder::clear() {
	m_inputAssembly = { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	m_rasterizer = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	m_colorBlendAttachment = {};
	m_multisampling = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	m_pipelineLayout = {};
	m_depthStencil = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	m_renderInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	m_shaderStages.clear();
}

VkPipeline PipelineBuilder::build_pipeline(VkDevice device) {
	
	// Make the viewport state from our stored viewport and scissor.
	// At the moment we wont support multiple viewports or scissors.
	VkPipelineViewportStateCreateInfo viewPortState{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pNext = nullptr,
		.viewportCount = 1,  // Don't need to fill the viewport and scissor stuff here since we are
		.scissorCount = 1    // Using a dynamic viewport 
	};

	// Dummy color blending, we arent using transparent objects yet
	// The blending is just "no blend", but we do write to the color attachment
	VkPipelineColorBlendStateCreateInfo colorBlending{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = nullptr,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &m_colorBlendAttachment
	};

	// Clear vertex input state since we aren't using it
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
	};

	// Now we setup the dynamic state
	VkDynamicState state[] = {
		VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = 2,
		.pDynamicStates = &state[0],
	};

	VkGraphicsPipelineCreateInfo pipelineInfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &m_renderInfo,
		.stageCount = static_cast<uint32_t>(m_shaderStages.size()),
		.pStages = m_shaderStages.data(),
		.pVertexInputState = &vertexInputInfo,
		.pInputAssemblyState = &m_inputAssembly,
		.pViewportState = &viewPortState,
		.pRasterizationState = &m_rasterizer,
		.pMultisampleState = &m_multisampling,
		.pDepthStencilState = &m_depthStencil,
		.pColorBlendState = &colorBlending,
		.pDynamicState = &dynamicInfo, // Link the dynanmic state to our pipeline info
		.layout = m_pipelineLayout
	};

	VkPipeline newPipeline{};
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
		&pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
		fmt::println("Failed to create pipeline");
		return VK_NULL_HANDLE;
	} else {
		return newPipeline;
	}
}

void PipelineBuilder::set_pipeline_layout(VkPipelineLayout pipelineLayout) {
	m_pipelineLayout = pipelineLayout;
}

void PipelineBuilder::set_shaders(VkShaderModule vertexShader, VkShaderModule fragmentShader) {
	m_shaderStages.clear();

	// Push the vertex shader
	m_shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vertexShader)
	);

	// Push the fragment shader
	m_shaderStages.push_back(
		vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader)
	);
}

void PipelineBuilder::set_input_topology(VkPrimitiveTopology topology) {
	m_inputAssembly.topology = topology;

	// Tutorial doesn't use primitive restart so we set to false
	m_inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void PipelineBuilder::set_polygon_mode(VkPolygonMode mode) {
	m_rasterizer.polygonMode = mode;
	m_rasterizer.lineWidth = 1.f;
}

void PipelineBuilder::set_cull_mode(VkCullModeFlags cullMode, VkFrontFace frontFace) {
	m_rasterizer.cullMode = cullMode;
	m_rasterizer.frontFace = frontFace;
}

void PipelineBuilder::set_mulitsampling_none() {
	m_multisampling.sampleShadingEnable = VK_FALSE;
	// Multisampling default to one sample per bit
	m_multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	m_multisampling.minSampleShading = 1.0f;
	m_multisampling.pSampleMask = nullptr;
	// No alpha coverage
	m_multisampling.alphaToCoverageEnable = VK_FALSE;
	m_multisampling.alphaToOneEnable = VK_FALSE;
}

void PipelineBuilder::disable_blending() {
	// Default write mask
	m_colorBlendAttachment.colorWriteMask =
		  VK_COLOR_COMPONENT_R_BIT
		| VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT
		| VK_COLOR_COMPONENT_A_BIT;

	// No blending
	m_colorBlendAttachment.blendEnable = VK_FALSE;
}

void PipelineBuilder::set_color_attachment(VkFormat format) {
	m_colorAttachmentFormat = format;
	// Connect the format to the renderInfo structure
	m_renderInfo.colorAttachmentCount = 1;
	m_renderInfo.pColorAttachmentFormats = &m_colorAttachmentFormat;
}

void PipelineBuilder::set_depth_format(VkFormat format) {
	m_renderInfo.depthAttachmentFormat = format;
}

void PipelineBuilder::disable_depthtest() {
	m_depthStencil = depth_disabled_state();
}


