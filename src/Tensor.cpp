#include "minerva/Tensor.hpp"

#include "vk/Buffer.hpp"

namespace minerva {

uint64_t TensorImp::size_bytes() const {
	return _size;
}

TensorImp::TensorImp(const ContextHandle& context, uint64_t size)
	: _size(size)
	, buffer(createBuffer(
		context,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
		VK_BUFFER_USAGE_TRANSFER_DST_BIT |
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		0
	))
{}
TensorImp::~TensorImp() = default;

}
