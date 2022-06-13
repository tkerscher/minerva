#include "minerva/Uniform.hpp"

#include <cstring>

#include "vk/Buffer.hpp"

namespace minerva {

void* UniformImp::getData() const noexcept {
	return buffer->allocInfo.pMappedData;
}

const vulkan::Buffer& UniformImp::getBuffer() const noexcept {
	return *buffer;
}

UniformImp::UniformImp(const ContextHandle& context, uint64_t size)
	: buffer(vulkan::createBuffer(context, size,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
		VMA_ALLOCATION_CREATE_MAPPED_BIT,
		VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE))
{}

UniformImp::UniformImp(const ContextHandle& context, span<const std::byte> data)
	: UniformImp(context, data.size_bytes())
{
	memcpy(getData(), data.data(), data.size_bytes());
}

UniformImp::UniformImp(UniformImp&&) noexcept = default;
UniformImp& UniformImp::operator=(UniformImp&&) noexcept = default;

UniformImp::~UniformImp() = default;

}
