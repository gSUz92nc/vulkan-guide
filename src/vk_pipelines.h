#pragma once 
#include <vk_types.h>

namespace vkutil {
	
	bool load_shader_module(const char* filePath, VkDevice device, VkShaderModule* outShaderModule);


};

class PipelineBuilder {
private:
	std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages{};

	VkPipelineInputAssemblyStateCreateInfo m_inputAssembly{};
	VkPipelineRasterizationStateCreateInfo m_rasterizer{};
	VkPipelineColorBlendAttachmentState m_colorBlendAttachment{};
	VkPipelineMultisampleStateCreateInfo m_multisampling{};
	VkPipelineLayout m_pipelineLayout{};
	VkPipelineDepthStencilStateCreateInfo m_depthStencil{};
	VkPipelineRenderingCreateInfo m_renderInfo{};
	VkFormat m_colorAttachmentFormat{};

public:

	PipelineBuilder() {
	clear();
	}

	void clear();

	VkPipeline build_pipeline(VkDevice device);
	void set_pipeline_layout(VkPipelineLayout pipelineLayout);
	void set_shaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
	void set_input_topology(VkPrimitiveTopology topology);
	void set_polygon_mode(VkPolygonMode mode);
	void set_cull_mode(VkCullModeFlags cullMode, VkFrontFace frontFace);
	void set_mulitsampling_none();
	void disable_blending();
	void set_color_attachment(VkFormat format);
	void set_depth_format(VkFormat format);
	void disable_depthtest();
};
