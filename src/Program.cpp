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
	case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
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

void Parameter::setArgument(const UniformImp& uniform, uint32_t binding) {
	//Check same context
	if (&uniform.getBuffer().context != &_pImpl->context)
		throw std::logic_error("Tensor and Parameter must originate from the same context!");
	auto& context = _pImpl->context;

	VkDescriptorBufferInfo bufferInfo{
		uniform.getBuffer().buffer,
		0,
		uniform.getBuffer().allocInfo.size
	};
	auto write = vulkan::WriteDescriptorSet(
		_pImpl->descriptorSet,
		binding,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
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
	CommandHandle cmd;

	const vulkan::Context& context;

	pImpl(vulkan::Context& context, uint32_t set_count)
		: context(context)
		, setLayouts(set_count)
		, pipeline(nullptr)
		, pipeLayout(nullptr)
		, shader(nullptr)
		, cmd({ nullptr, nullptr })
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
	return static_cast<uint32_t>(_pImpl->setLayouts.size());
}

void Program::beginCommand() {
	//create command
	_pImpl->cmd = vulkan::createCommand(_pImpl->context);
	//bind pipeline
	_pImpl->context.table.vkCmdBindPipeline(
		_pImpl->cmd->buffer, VK_PIPELINE_BIND_POINT_COMPUTE, _pImpl->pipeline);
}

void Program::bindParams(span<const Parameter> params) {
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
	_pImpl->context.table.vkCmdBindDescriptorSets(_pImpl->cmd->buffer,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		_pImpl->pipeLayout,
		0, static_cast<uint32_t>(sets.size()), sets.data(),
		0, nullptr);
}

void Program::dispatch(const Dispatch<void>& dispatch) {
	//push constant if there is any
	if (!dispatch.push.empty()) {
		_pImpl->context.table.vkCmdPushConstants(_pImpl->cmd->buffer,
			_pImpl->pipeLayout,
			VK_SHADER_STAGE_COMPUTE_BIT,
			0, static_cast<uint32_t>(dispatch.push.size_bytes()), dispatch.push.data());
	}

	//dispatch
	_pImpl->context.table.vkCmdDispatch(_pImpl->cmd->buffer,
		dispatch.groupCountX, dispatch.groupCountY, dispatch.groupCountZ);
}

CommandHandle Program::endCommand() {
	//end recording & return
	vulkan::checkResult(_pImpl->context.table.vkEndCommandBuffer(_pImpl->cmd->buffer));
	return std::move(_pImpl->cmd);
}

Program::Program(const ContextHandle& context, span<const uint32_t> code, span<const std::byte> consts) {
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

	auto layoutInfo = vulkan::PipelineLayoutCreateInfo(_pImpl->setLayouts);

	//read push descriptor
	VkPushConstantRange push = {};
	if (reflectModule->push_constant_block_count > 1) {
		throw std::logic_error("Multiple push constant found, but only up to one is supported!");
	}
	else if (reflectModule->push_constant_block_count == 1) {
		push.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		push.size = reflectModule->push_constant_blocks->size; //padded_size?

		layoutInfo.pushConstantRangeCount = 1;
		layoutInfo.pPushConstantRanges = &push;
	}

	//create pipeline layout
	vulkan::checkResult(context->table.vkCreatePipelineLayout(
		context->device, &layoutInfo, nullptr, &_pImpl->pipeLayout));

	//compile shader into module
	auto shaderInfo = vulkan::ShaderModuleCreateInfo(code);
	vulkan::checkResult(context->table.vkCreateShaderModule(
		context->device, &shaderInfo, nullptr, &_pImpl->shader));

	//create specialization constants
	//spirv reflect does not support spec constant and even if we'd still have to guess the mapping
	//instead we assume all spec consts to be 4 bytes starting with id 1 in consecutive order
	//without jumping any ids

	auto constCount = consts.size_bytes() / 4;
	std::vector<VkSpecializationMapEntry> specMap(constCount);
	for (auto i = 0u; i < constCount; ++i) {
		specMap[i] = { i, 4 * i, 4 };
	}
	auto specInfo = vulkan::SpecializationInfo(specMap, consts);

	//Create compute pipeline
	auto pipeInfo = vulkan::ComputePipelineCreateInfo(
		_pImpl->pipeLayout,
		reflectModule->entry_point_name,
		_pImpl->shader,
		consts.empty() ? nullptr : &specInfo);
	vulkan::checkResult(context->table.vkCreateComputePipelines(
		context->device, context->pipeCache,
		1, &pipeInfo,
		nullptr, &_pImpl->pipeline));
}

Program::Program(const ContextHandle& context, const char* filename, span<const std::byte> consts)
	: Program(context, loadShader(filename), consts)
{}

Program::Program(const ContextHandle& context, span<const uint32_t> code)
	: Program(context, code, {})
{}

Program::Program(const ContextHandle& context, const char* filename)
	: Program(context, loadShader(filename), {})
{}

Program::~Program() = default;

/******************************************** Util ****************************************/

std::vector<uint32_t> loadShader(const char* filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (!file)
		throw std::runtime_error("File does not exist or cant be read!");
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
