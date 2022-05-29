#include "minerva/Image.hpp"

#include <stdexcept>

#include "volk.h"

#include "vk/Image.hpp"

namespace minerva {

uint32_t Image::getWidth() const {
	return _width;
}
uint32_t Image::getHeight() const {
	return _height;
}
uint32_t Image::getDepth() const {
	return _depth;
}

ImageFormat Image::getFormat() const {
	return _format;
}
uint64_t Image::size_bytes() const {
	return getElementSize(getFormat()) * getWidth() * getHeight() * getDepth();
}

const vulkan::Image& Image::getImage() const {
	return *image;
}

Image::Image(const ContextHandle& context,
	ImageFormat format,
	uint32_t width)
	: image(vulkan::createImage(*context, static_cast<VkFormat>(format), width))
	, _format(format)
	, _width(width), _height(1), _depth(1)
{}

Image::Image(const ContextHandle& context,
	ImageFormat format,
	uint32_t width, uint32_t height)
	: image(vulkan::createImage(*context, static_cast<VkFormat>(format), width, height))
	, _format(format)
	, _width(width), _height(height), _depth(1)
{}

Image::Image(const ContextHandle& context,
	ImageFormat format,
	uint32_t width, uint32_t height, uint32_t depth)
	: image(vulkan::createImage(*context, static_cast<VkFormat>(format), width, height, depth))
	, _format(format)
	, _width(width), _height(height), _depth(depth)
{}

Image::~Image() = default;

#define ELEMENT_SIZE(x) \
case ImageFormat::x: \
	return sizeof(ImageElementTypeValue<ImageFormat::x>);

uint64_t getElementSize(ImageFormat format) {
	switch (format)
	{
		ELEMENT_SIZE(R8G8B8A8_UNORM)
		ELEMENT_SIZE(R8G8B8A8_SNORM)
		ELEMENT_SIZE(R8G8B8A8_UINT)
		ELEMENT_SIZE(R8G8B8A8_SINT)
		ELEMENT_SIZE(R16G16B16A16_UINT)
		ELEMENT_SIZE(R16G16B16A16_SINT)
		ELEMENT_SIZE(R32_UINT)
		ELEMENT_SIZE(R32_SINT)
		ELEMENT_SIZE(R32_SFLOAT)
		ELEMENT_SIZE(R32G32_UINT)
		ELEMENT_SIZE(R32G32_SINT)
		ELEMENT_SIZE(R32G32_SFLOAT)
		ELEMENT_SIZE(R32G32B32A32_UINT)
		ELEMENT_SIZE(R32G32B32A32_SINT)
		ELEMENT_SIZE(R32G32B32A32_SFLOAT)
		ELEMENT_SIZE(R8_UNORM)
		ELEMENT_SIZE(R8_SNORM)
		ELEMENT_SIZE(R8_UINT)
		ELEMENT_SIZE(R8_SINT)
		ELEMENT_SIZE(R8G8_UNORM)
		ELEMENT_SIZE(R8G8_SNORM)
		ELEMENT_SIZE(R8G8_UINT)
		ELEMENT_SIZE(R8G8_SINT)
		ELEMENT_SIZE(R16_UNORM)
		ELEMENT_SIZE(R16_SNORM)
		ELEMENT_SIZE(R16_UINT)
		ELEMENT_SIZE(R16_SINT)
		ELEMENT_SIZE(R16G16_UNORM)
		ELEMENT_SIZE(R16G16_SNORM)
		ELEMENT_SIZE(R16G16_UINT)
		ELEMENT_SIZE(R16G16_SINT)
		ELEMENT_SIZE(R16G16B16A16_UNORM)
		ELEMENT_SIZE(R16G16B16A16_SNORM)
	default:
		throw std::logic_error("Unknown image format!");
	}
}

#undef ELEMENT_SIZE

}
