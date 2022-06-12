#pragma once

#include <memory>

namespace minerva {
	//forward internal
	namespace vulkan {
		struct Buffer;
		struct Command;
		struct Context;
		struct Instance;
		struct Image;
		struct Timeline;
	}

	/**
	 * @brief Handle to a Vulkan buffer
	 * @ingroup memory
	 */
	using BufferHandle = std::unique_ptr<vulkan::Buffer, void(*)(vulkan::Buffer*)>;
	/**
	 * @brief Handle to a Vulkan command buffer
	 * @ingroup command
	 */
	using CommandHandle = std::unique_ptr<vulkan::Command, void(*)(vulkan::Command*)>;
	/**
	 * @brief Handle to a context
	 * @ingroup core
	 */
	using ContextHandle = std::unique_ptr<vulkan::Context, void(*)(vulkan::Context*)>;
	/**
	 * @brief Handle to a Vulkan instance
	 * @ingroup core
	 */
	using InstanceHandle = std::unique_ptr<vulkan::Instance, void(*)(vulkan::Instance*)>;
	/**
	 * @brief Handle to a Vulkan image
	 * @ingroup memory
	 */
	using ImageHandle = std::unique_ptr<vulkan::Image, void(*)(vulkan::Image*)>;
	/**
	 * @brief Handle to a Vulkan timeline
	 * @ingroup command
	 */
	using TimelineHandle = std::unique_ptr<vulkan::Timeline, void(*)(vulkan::Timeline*)>;
}
