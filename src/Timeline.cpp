#include "minerva/Timeline.hpp"

#include "volk.h"

#include "vk/Context.hpp"
#include "vk/Structs.hpp"
#include "vk/Timeline.hpp"
#include "vk/result.hpp"

namespace minerva {

uint64_t Timeline::getValue() {
	uint64_t value;
	vulkan::checkResult(timeline->context.table.vkGetSemaphoreCounterValue(
		timeline->context.device, timeline->semaphore, &value));
	return value;
}

void Timeline::setValue(uint64_t value) {
	auto info = vulkan::SemaphoreSignalInfo(timeline->semaphore, value);
	vulkan::checkResult(timeline->context.table.vkSignalSemaphore(
		timeline->context.device, &info));
}

void Timeline::waitValue(uint64_t value) const {
	waitValue(value, UINT64_MAX);
}

bool Timeline::waitValue(uint64_t value, uint64_t timeout) const {
	auto info = vulkan::SemaphoreWaitInfo(timeline->semaphore, value);
	auto result = timeline->context.table.vkWaitSemaphores(
		timeline->context.device, &info, timeout);
	switch (result)	{
	case VK_SUCCESS:
		return true;
	case VK_TIMEOUT:
		return false;
	default:
		vulkan::checkResult(result); //will throw
		return false;
	}
}

Timeline::Timeline(const ContextHandle& context, uint64_t initialValue)
	: timeline(createTimeline(context, initialValue))
{}

}
