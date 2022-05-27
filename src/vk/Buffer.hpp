#pragma once

#define VK_NO_PROTOTYPES
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include "vk_mem_alloc.h"

#include "minerva/Handles.hpp"

namespace minerva::vulkan {
	struct Buffer {
		VkBuffer buffer;
		VmaAllocation allocation;
		VmaAllocationInfo allocInfo;

		const Context& context;
	};

	[[nodiscard]]
	BufferHandle createBuffer(
		const ContextHandle& context,
		uint64_t size,
		VkBufferUsageFlags usage,
		VmaAllocationCreateFlags flags);
	void destroyBuffer(Buffer* buffer);
}
