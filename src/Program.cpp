#include "minerva/Program.hpp"

#include <fstream>
#include <memory>
#include <stdexcept>

#include "volk.h"
#include "spirv_reflect.h"

#include "minerva/Tensor.hpp"

#include "vk/Buffer.hpp"
#include "vk/Command.hpp"
#include "vk/Context.hpp"
#include "vk/Image.hpp"
#include "vk/Structs.hpp"
#include "vk/result.hpp"

//helper
void spvCheckResult(SpvReflectResult result) {
	if (result != SPV_REFLECT_RESULT_SUCCESS)
		throw std::runtime_error("Failed to retrieve reflection information from the shader!");
}
bool descriptorTypeSupported(SpvReflectDescriptorType type) {
	switch (type) {
	case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
	case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
		return true;
	default:
		return false;
	}
}

namespace minerva {

namespace vulkan {

[[nodiscard]]
constexpr VkDescriptorSetLayoutBinding DesciptorSetLayoutBinding(
	SpvReflectDescriptorBinding binding)
{
	VkDescriptorSetLayoutBinding result = {};

	result.binding = binding.binding;
	result.descriptorCount = binding.count;
	result.descriptorType = static_cast<VkDescriptorType>(binding.descriptor_type);
	result.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	return result;
}

}

/**************************************** Parameter ***********************************/

struct Parameter::pImpl {
	VkDescriptorSet descriptorSet;
	const vulkan::Context& context;

	pImpl(const vulkan::Context& context)
		: context(context)
		, descriptorSet(nullptr)
	{}
	~pImpl() {
		context.table.vkFreeDescriptorSets(context.device,
			context.dscPool, 1, &descriptorSet);
	}
};

void Parameter::setArgument(const TensorImp& tensor, uint32_t binding) {
	//Check same context
	if (&tensor.getBuffer().context != &_pImpl->context)
		throw std::logic_error("Tensor and Parameter must originate from the same context!");
	auto& context = _pImpl->context;

	VkDescriptorBufferInfo bufferInfo{
		tensor.getBuffer().buffer, //buffer
		0,                         //offset
		tensor.size_bytes()        //range
	};
	auto write = vulkan::WriteDescriptorSet(
		_pImpl->descriptorSet,
		binding,
		VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		bufferInfo);

	context.table.vkUpdateDescriptorSets(context.device,
		1, &write,
		0, nullptr);
}

void Parameter::setArgument(const Image& img, uint32_t binding) {
	//Check same context
	if (&img.getImage().context != &_pImpl->context)
		throw std::logic_error("Tensor and Parameter must originate from the same context!");
	auto& context = _pImpl->context;

	VkDescriptorImageInfo imageInfo{
		VK_NULL_HANDLE,
		img.getImage().view,
		VK_IMAGE_LAYOUT_GENERAL
	};
	auto write = vulkan::WriteDescriptorSet(
		_pImpl->descriptorSet,
		binding,
		VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		imageInfo);

	context.table.vkUpdateDescriptorSets(context.device,
		1, &write,
		0, nullptr);
}

Parameter::Parameter(std::unique_ptr<pImpl> pImpl)
	: _pImpl(std::move(pImpl))
{}

Parameter::~Parameter() = default;

/***************************************** Program ************************************/

struct Program::pImpl {
	std::vector<VkDescriptorSetLayout> setLayouts;
	VkPipeline pipeline;
	VkPipelineLayout pipeLayout;
	VkShaderModule shader;

	const vulkan::Context& context;

	pImpl(vulkan::Context& context, uint32_t set_count)
		: context(context)
		, setLayouts(set_count)
		, pipeline(nullptr)
		, pipeLayout(nullptr)
		, shader(nullptr)
	{}
	~pImpl() {
		//destroy in reverse order
		context.table.vkDestroyPipeline(context.device, pipeline, nullptr);
		context.table.vkDestroyShaderModule(context.device, shader, nullptr);
		context.table.vkDestroyPipelineLayout(context.device, pipeLayout, nullptr);
		for (auto& layout : setLayouts) {
			context.table.vkDestroyDescriptorSetLayout(context.device, layout, nullptr);
		}
	}
};

Parameter Program::createParameter(uint32_t set) const {
	if (set >= _pImpl->setLayouts.size())
		throw std::logic_error("The given set is out of range!");

	auto _p = std::make_unique<Parameter::pImpl>(_pImpl->context);
	auto info = vulkan::DescriptorSetAllocateInfo(
		_pImpl->context.dscPool, _pImpl->setLayouts[set]);
	vulkan::checkResult(_pImpl->context.table.vkAllocateDescriptorSets(
		_pImpl->context.device, &info, &_p->descriptorSet));

	return { std::move(_p) };
}

uint32_t Program::getParameterCount() const {
	return _pImpl->setLayouts.size();
}

CommandHandle Program::Run(uint32_t x, uint32_t y, uint32_t z) {
	return Run(x, y, z, {});
}

CommandHandle Program::Run(uint32_t x, uint32_t y, uint32_t z, const Parameter& param) {
	return Run(x, y, z, { &param, 1 });
}

CommandHandle Program::Run(uint32_t x, uint32_t y, uint32_t z, span<const Parameter> params) {
	//sanity check
	if (params.size() != getParameterCount())
		throw std::logic_error("Not all parameters provided!");

	//Create command buffer
	auto cmd = vulkan::createCommand(_pImpl->context);
	
	//bind pipeline
	_pImpl->context.table.vkCmdBindPipeline(
		cmd->buffer, VK_PIPELINE_BIND_POINT_COMPUTE, _pImpl->pipeline);

	//collect sets
	std::vector<VkDescriptorSet> sets(params.size());
	auto i = 0u;
	for (auto& p : params) {
		//TODO: Check if the Parameter originate from this Program
		if (&p._pImpl->context != &_pImpl->context)
			throw std::logic_error("All Parameter must originate from the same context!");

		sets[i++] = p._pImpl->descriptorSet;
	}
	//bind sets
	_pImpl->context.table.vkCmdBindDescriptorSets(cmd->buffer,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		_pImpl->pipeLayout,
		0, sets.size(), sets.data(),
		0, nullptr);
	
	//dispatch
	_pImpl->context.table.vkCmdDispatch(cmd->buffer, x, y, z);

	//end recording and return
	vulkan::checkResult(_pImpl->context.table.vkEndCommandBuffer(cmd->buffer));
	return cmd;
}

Program::Program(const ContextHandle& context, span<uint32_t> code) {
	//RAII wrapper for SpvReflectModule
	using ReflectHandle = std::unique_ptr<SpvReflectShaderModule, decltype(&spvReflectDestroyShaderModule)>;
	auto reflectModule = ReflectHandle(new SpvReflectShaderModule, spvReflectDestroyShaderModule);
	//create reflection
	spvCheckResult(spvReflectCreateShaderModule2(
		SPV_REFLECT_MODULE_FLAG_NO_COPY,
		code.size_bytes(), code.data(),
		reflectModule.get()));

	//We can now see how many layouts we will create
	// -> init the pimpl
	_pImpl = std::make_unique<Program::pImpl>(*context, reflectModule->descriptor_set_count);

	//build sets and layouts
	for (auto iSet = 0u; iSet < reflectModule->descriptor_set_count; ++iSet) {
		auto& set = reflectModule->descriptor_sets[iSet];

		//Create bindings
		auto bndPtr = *set.bindings;
		std::vector<VkDescriptorSetLayoutBinding> bindings(set.binding_count);
		for (auto iBnd = 0u; iBnd < set.binding_count; ++iBnd, ++bndPtr) {
			if (!descriptorTypeSupported(bndPtr->descriptor_type))
				throw std::logic_error("Encountered unsupported descriptor type while parsing shader!");
			bindings[iBnd] = vulkan::DesciptorSetLayoutBinding(*bndPtr);
		}

		//create set layout
		auto createInfo = vulkan::DescriptorSetLayoutCreateInfo(bindings);
		vulkan::checkResult(context->table.vkCreateDescriptorSetLayout(
			context->device, &createInfo, nullptr, _pImpl->setLayouts.data() + iSet));
	}

	//create pipeline layout
	auto layoutInfo = vulkan::PipelineLayoutCreateInfo(_pImpl->setLayouts);
	vulkan::checkResult(context->table.vkCreatePipelineLayout(
		context->device, &layoutInfo, nullptr, &_pImpl->pipeLayout));

	//compile shader into module
	auto shaderInfo = vulkan::ShaderModuleCreateInfo(code);
	vulkan::checkResult(context->table.vkCreateShaderModule(
		context->device, &shaderInfo, nullptr, &_pImpl->shader));

	//Create compute pipeline
	auto pipeInfo = vulkan::ComputePipelineCreateInfo(
		_pImpl->pipeLayout, reflectModule->entry_point_name, _pImpl->shader);
	vulkan::checkResult(context->table.vkCreateComputePipelines(
		context->device, context->pipeCache,
		1, &pipeInfo,
		nullptr, &_pImpl->pipeline));
}

Program::Program(const ContextHandle& context, const char* filename)
	: Program(context, loadShader(filename))
{}

Program::~Program() = default;

/******************************************** Util ****************************************/

std::vector<uint32_t> loadShader(const char* filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	auto size = static_cast<size_t>(file.tellg());
	file.seekg(0, std::ios::beg);
	std::vector<uint32_t> code(size / 4);
	if (!file.read(reinterpret_cast<char*>(code.data()), size)) {
		//return empty vector
		code.clear();
	}

	return code;
}

}
