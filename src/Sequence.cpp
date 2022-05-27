#include "minerva/Sequence.hpp"

#include <stdexcept>
#include <vector>

#include "volk.h"

#include "vk/Command.hpp"
#include "vk/Context.hpp"
#include "vk/Structs.hpp"
#include "vk/Timeline.hpp"
#include "vk/result.hpp"

namespace minerva {

struct SequenceBuilder::pImp {
	std::vector<std::vector<VkCommandBuffer>> commands;
	std::vector<uint64_t> waitValues;
	std::vector<uint64_t> signalValues;
	//the value to wait for in the next batch. Might differ from signalValues.back()
	uint64_t currentValue;

	VkSemaphore timeline;

	const vulkan::Context& context;

	pImp(const vulkan::Context& context, VkSemaphore timeline, uint64_t value)
		: commands({})
		, timeline(timeline)
		, currentValue(value + 1)
		, context(context)
	{
		commands.push_back({});
		waitValues.push_back(value);
		signalValues.push_back(value + 1);
	}
};

SequenceBuilder& SequenceBuilder::And(const CommandHandle& command) {
	if (&command->context != &_pImp->context)
		throw std::logic_error("Timeline and command must originate from the same context!");

	_pImp->commands.back().push_back(command->buffer);
	return *this;
}

SequenceBuilder& SequenceBuilder::Then(const CommandHandle& command) {
	//add new level
	_pImp->commands.emplace_back();
	_pImp->waitValues.push_back(_pImp->currentValue);
	_pImp->currentValue++;
	_pImp->signalValues.push_back(_pImp->currentValue);

	return And(command);
}

SequenceBuilder& SequenceBuilder::WaitFor(uint64_t value) {
	if (value < _pImp->currentValue)
		throw std::logic_error("Wait value too low! Timeline can only go forward!");
	
	_pImp->currentValue = value;
	return *this;
}

uint64_t SequenceBuilder::Submit() {
	if (!_pImp)
		throw std::logic_error("A sequence can only be submitted once!");

	//build infos
	auto size = _pImp->commands.size();
	std::vector<VkTimelineSemaphoreSubmitInfo> timelineInfos(size);
	std::vector<VkSubmitInfo> submits(size);
	std::vector<VkPipelineStageFlags> waitFlags(size,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
	auto wPtr = _pImp->waitValues.data();
	auto sPtr = _pImp->signalValues.data();
	auto fPtr = waitFlags.data();
	auto tPtr = timelineInfos.data();
	for (auto i = 0u; i < submits.size(); ++i, ++wPtr, ++sPtr, ++fPtr, ++tPtr) {
		*tPtr = vulkan::TimelineSemaphoreSubmitInfo(wPtr, sPtr);
		auto submitInfo = vulkan::SubmitInfo(
			_pImp->timeline,
			fPtr,
			tPtr,
			_pImp->commands[i]);
		submits[i] = submitInfo;
	}

	//submit
	vulkan::checkResult(_pImp->context.table.vkQueueSubmit(
		_pImp->context.queue, submits.size(), submits.data(), nullptr));

	//return final signal value
	return _pImp->currentValue;
}

SequenceBuilder::SequenceBuilder(Timeline& timeline, uint64_t startValue) 
	: _pImp(new pImp(
		timeline.timeline->context,
		timeline.timeline->semaphore,
		startValue))
{}
SequenceBuilder::~SequenceBuilder() = default;

}
