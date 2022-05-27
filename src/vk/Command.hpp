#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include "minerva/Handles.hpp"

namespace minerva::vulkan {
	struct Command {
		VkCommandBuffer buffer;

		//bit ugly and dangerous, but better than creating a shared_ptr
		const Context& context;
	};

	[[nodiscard]]
	CommandHandle createCommand(const Context& handle);
	void destroyCommand(Command* command);
}
