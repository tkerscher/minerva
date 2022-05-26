#include "minerva/Buffer.hpp"

#include "vk/Buffer.hpp"
#include "vk/Context.hpp"

namespace minerva {

span<std::byte> BufferImp::getMemory() const {
	return memory;
}

BufferImp::BufferImp(const ContextHandle& context, uint64_t size)
	: buffer(createBuffer(
		context,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
		VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT
	))
{
	memory = span<std::byte>(static_cast<std::byte*>(buffer->allocInfo.pMappedData), size);
}

}
