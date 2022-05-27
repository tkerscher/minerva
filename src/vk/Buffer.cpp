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
	BufferHandle result{new Buffer({0,0,{},*context}), destroyBuffer };

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
		&result->allocInfo));

	return result;
}

void destroyBuffer(Buffer* buffer) {
	if (!buffer)
		return;

	vmaDestroyBuffer(buffer->context.allocator, buffer->buffer, buffer->allocation);
}

}
