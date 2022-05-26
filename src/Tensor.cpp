#include "minerva/Tensor.hpp"

#include "vk/Buffer.hpp"

namespace minerva {

TensorImp::TensorImp(const ContextHandle& context, uint64_t size)
	: buffer(createBuffer(
		context,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
		VK_BUFFER_USAGE_TRANSFER_DST_BIT |
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		0
	))
{}

}
