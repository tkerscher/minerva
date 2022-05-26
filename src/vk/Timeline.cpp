#include "vk/Timeline.hpp"

#include "volk.h"

#include "vk/Context.hpp"
#include "vk/Structs.hpp"
#include "vk/result.hpp"

namespace minerva::vulkan {

TimelineHandle createTimeline(const ContextHandle& context, uint64_t initialValue) {
	TimelineHandle result{
		new Timeline({nullptr, *context}),
		destroyTimeline
	};
	
	auto type = SemaphoreTypeCreateInfo(initialValue);
	auto info = SemaphoreCreateInfo(type);
	checkResult(context->table.vkCreateSemaphore(
		context->device, &info, nullptr, &result->semaphore));

	return result;
}

void destroyTimeline(Timeline* timeline) {
	if (!timeline)
		return;

	timeline->context.table.vkDestroySemaphore(
		timeline->context.device, timeline->semaphore, nullptr);
}

}