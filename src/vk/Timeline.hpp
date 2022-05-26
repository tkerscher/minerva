#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include "minerva/Handles.hpp"

namespace minerva::vulkan {
	struct Timeline {
		VkSemaphore semaphore;

		//reference to context
		//bit dangerous and ugly, but shared_ptr seemed to be to heavy
		const Context& context;
	};

	[[nodiscard]]
	TimelineHandle createTimeline(const ContextHandle& context, uint64_t initialValue = 0);
	void destroyTimeline(Timeline* timeline);
}
