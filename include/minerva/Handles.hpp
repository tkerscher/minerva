#pragma once

#include <memory>

namespace minerva {
	//forward internal
	namespace vulkan {
		struct Buffer;
		struct Context;
		struct Instance;
	}

	using BufferHandle = std::unique_ptr<vulkan::Buffer, void(*)(vulkan::Buffer*)>;
	using ContextHandle = std::unique_ptr<vulkan::Context, void(*)(vulkan::Context*)>;
	using InstanceHandle = std::unique_ptr<vulkan::Instance, void(*)(vulkan::Instance*)>;
}
