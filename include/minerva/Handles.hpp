#pragma once

#include <memory>

namespace minerva {
	//forward internal
	namespace vulkan {
		struct Buffer;
		struct Command;
		struct Context;
		struct Instance;
		struct Timeline;
	}

	using BufferHandle = std::unique_ptr<vulkan::Buffer, void(*)(vulkan::Buffer*)>;
	using CommandHandle = std::unique_ptr<vulkan::Command, void(*)(vulkan::Command*)>;
	using ContextHandle = std::unique_ptr<vulkan::Context, void(*)(vulkan::Context*)>;
	using InstanceHandle = std::unique_ptr<vulkan::Instance, void(*)(vulkan::Instance*)>;
	using TimelineHandle = std::unique_ptr<vulkan::Timeline, void(*)(vulkan::Timeline*)>;
}
