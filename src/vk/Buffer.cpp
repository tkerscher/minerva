#include "vk/Buffer.hpp"

#include "vk/Context.hpp"
#include "vk/Structs.hpp"
#include "vk/result.hpp"

namespace minerva::vulkan {

BufferHandle createBuffer(
	const ContextHandle& context,
	uint64_t size,
	VkBufferUsageFlags usage,
	VmaAllocationCreateFlags flags)
{
	BufferHandle result{ new Buffer(), destroyBuffer };
	result->allocator = context->allocator;

	auto bufferInfo = BufferCreateInfo(size, usage);
	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocInfo.flags = flags;

	checkResult(vmaCreateBuffer(
		context->allocator,
		&bufferInfo,
		&allocInfo,
		&result->buffer,
		&result->allocation,
		nullptr));

	return result;
}

void destroyBuffer(Buffer* buffer) {
	if (!buffer)
		return;

	vmaUnmapMemory(buffer->allocator, buffer->allocation);
	vmaDestroyBuffer(buffer->allocator, buffer->buffer, buffer->allocation);
}

}
